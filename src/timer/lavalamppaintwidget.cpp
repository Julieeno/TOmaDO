
#include "timer/lavalamppaintwidget.h"
#include <QPaintEvent>
#include <QResizeEvent>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QRandomGenerator>
#include <QtMath>
#include <QPainterPath>
#include <algorithm>
#include <QDebug>

LavaLampPaintWidget::LavaLampPaintWidget(QWidget* parent)
    : QWidget(parent)
    , m_animationTimer(new QTimer(this))
    , m_primaryColor(255, 60, 20, 240)
    , m_secondaryColor(255, 140, 0, 220)
    , m_backgroundColor(8, 15, 35)
    , m_frameCount(0)
    , m_borderRadius(0)
    , m_temperature(1.0)
    , m_heatSourceIntensity(1.0)
    , m_temperatureBasedColors(false)
    , m_scaleFactor(1.0)  // Initialize scale factor
    , m_baseMinSize(BASE_MIN_BLOB_SIZE)
    , m_baseMaxSize(BASE_MAX_BLOB_SIZE)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAutoFillBackground(false);

    connect(m_animationTimer, &QTimer::timeout, this, &LavaLampPaintWidget::updateAnimation);

    calculateScaleFactor();
    initializeBlobs();
}

void LavaLampPaintWidget::calculateScaleFactor()
{
    if (width() <= 0 || height() <= 0) {
        m_scaleFactor = 1.0;
        return;
    }

    // Calculate scale factor based on widget area
    // Use a reference size (e.g., 400x600) as baseline
    const qreal referenceWidth = 400.0;
    const qreal referenceHeight = 600.0;
    const qreal referenceArea = referenceWidth * referenceHeight;

    qreal currentArea = width() * height();
    qreal areaRatio = currentArea / referenceArea;

    // Apply square root to make scaling more gradual
    m_scaleFactor = std::sqrt(areaRatio);

    // Clamp the scale factor to reasonable bounds
    m_scaleFactor = std::max(MIN_SCALE_FACTOR, std::min(MAX_SCALE_FACTOR, m_scaleFactor));

    // Update the actual size ranges based on scale factor
    m_baseMinSize = BASE_MIN_BLOB_SIZE * m_scaleFactor;
    m_baseMaxSize = BASE_MAX_BLOB_SIZE * m_scaleFactor;
}

void LavaLampPaintWidget::setTemperatureBasedColors(bool enabled)
{
    m_temperatureBasedColors = enabled;

    // Reset all blob colors to primary/secondary if disabling temperature-based colors
    if (!enabled) {
        for (size_t i = 0; i < m_blobs.size(); ++i) {
            m_blobs[i].color = (i % 2 == 0) ? m_primaryColor : m_secondaryColor;
        }
    }

    update();
}

void LavaLampPaintWidget::setBorderRadius(int radius)
{
    m_borderRadius = radius;
    update();
}

void LavaLampPaintWidget::startAnimation()
{
    if (!m_animationTimer->isActive()) {
        m_animationTimer->start(ANIMATION_INTERVAL);
    }
}

void LavaLampPaintWidget::stopAnimation()
{
    m_animationTimer->stop();
}

void LavaLampPaintWidget::setColorScheme(const QColor& primaryColor, const QColor& secondaryColor, const QColor& backgroundColor)
{
    m_primaryColor = primaryColor;
    m_secondaryColor = secondaryColor;
    m_backgroundColor = backgroundColor;

    for (size_t i = 0; i < m_blobs.size(); ++i) {
        m_blobs[i].color = (i % 2 == 0) ? m_primaryColor : m_secondaryColor;
    }

    update();
}

void LavaLampPaintWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (m_borderRadius > 0) {
        QPainterPath clipPath;
        clipPath.addRoundedRect(rect(), m_borderRadius, m_borderRadius);
        painter.setClipPath(clipPath);
    }

    createLavaLampBackground(painter);
    drawThermalEffects(painter);
    drawBlobGlows(painter);
    drawLiquidBlobs(painter);
    drawBlobHighlights(painter);
    drawGlassEffect(painter);
}

void LavaLampPaintWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    calculateScaleFactor();
    initializeBlobs();
}

void LavaLampPaintWidget::updateAnimation()
{
    updateBlobs();
    updateThermalDynamics();
    update();
    ++m_frameCount;
}

qreal LavaLampPaintWidget::randomDouble(qreal min, qreal max)
{
    auto* rng = QRandomGenerator::global();
    return min + (max - min) * rng->generateDouble();
}

void LavaLampPaintWidget::initializeBlobs()
{
    m_blobs.clear();
    m_blobs.reserve(BLOB_COUNT);

    if (width() <= 0 || height() <= 0) {
        return;
    }

    for (int i = 0; i < BLOB_COUNT; ++i) {
        LavaBlob blob;

        bool isRising = (i % 3 != 0);

        if (isRising) {
            blob.position = QPointF(
                randomDouble(-width() * 0.2, width() * 1.2),
                height() + randomDouble(10, 60)
            );
            blob.velocity = QPointF(
                randomDouble(-0.3, 0.3),
                randomDouble(-0.7, -0.4)
            );
            blob.density = randomDouble(0.75, 0.85);
            blob.temperature = randomDouble(0.85, 1.0);
        } else {
            blob.position = QPointF(
                randomDouble(-width() * 0.2, width() * 1.2),
                randomDouble(-60, -10)
            );
            blob.velocity = QPointF(
                randomDouble(-0.3, 0.3),
                randomDouble(0.4, 0.7)
            );
            blob.density = randomDouble(1.15, 1.25);
            blob.temperature = randomDouble(0.1, 0.3);
        }

        // Use responsive size ranges
        blob.baseSize = randomDouble(m_baseMinSize, m_baseMaxSize);
        blob.size = blob.baseSize;
        blob.originalSize = blob.baseSize;

        blob.color = (i % 2 == 0) ? m_primaryColor : m_secondaryColor;
        blob.alpha = 0.85;
        blob.phase = randomDouble(0.0, 2.0 * M_PI);
        blob.pulseSpeed = randomDouble(0.003, 0.007);
        blob.temperaturePhase = randomDouble(0.0, 2.0 * M_PI);
        blob.lifetime = 0;
        blob.maxLifetime = QRandomGenerator::global()->bounded(500, 1000);
        blob.isRising = isRising;
        blob.mergeTimer = 0;
        blob.surfaceTension = randomDouble(0.06, 0.12);
        blob.viscosity = randomDouble(0.96, 0.99);
        blob.thermalConductivity = randomDouble(0.04, 0.10);

        initializeBlobShape(blob);
        m_blobs.push_back(blob);
    }
}

void LavaLampPaintWidget::initializeBlobShape(LavaBlob& blob)
{
    const int numPoints = 16;
    blob.controlPoints.clear();
    blob.controlPoints.reserve(numPoints);

    for (int i = 0; i < numPoints; ++i) {
        qreal angle = (2.0 * M_PI * i) / numPoints;
        qreal radiusVariation = randomDouble(0.95, 1.05);
        QPointF point(
            std::cos(angle) * blob.size * radiusVariation,
            std::sin(angle) * blob.size * radiusVariation
        );
        blob.controlPoints.push_back(point);
    }
}

void LavaLampPaintWidget::updateBlobs()
{
    if (m_blobs.empty()) {
        initializeBlobs();
        return;
    }

    // Improved physics parameters for more fluid movement
    const qreal heightScale = height() / 400.0;
    const qreal gravity = 0.008 * heightScale;        // Reduced for smoother movement
    const qreal buoyancy = 0.045 * heightScale;       // Reduced for more realistic buoyancy
    const qreal horizontalDrift = 0.001;              // Reduced for less erratic movement
    const qreal fluidDamping = 0.985;                 // Improved fluid damping

    for (auto& blob : m_blobs) {
        blob.lifetime++;

        if (blob.mergeTimer > 0) {
            blob.mergeTimer--;
        }

        updateBlobTemperature(blob);

        // Improved thermal dynamics
        qreal thermalBuoyancy = buoyancy * blob.temperature * blob.temperature; // Quadratic for more realistic effect
        qreal thermalGravity = gravity * (2.5 - blob.temperature);

        if (blob.isRising) {
            blob.velocity.ry() -= thermalBuoyancy / (blob.density * blob.density);
        } else {
            blob.velocity.ry() += thermalGravity * blob.density;
        }

        // Smoother thermal currents
        qreal thermalCurrent = std::sin(m_frameCount * 0.0015 + blob.temperaturePhase) * 0.02 * blob.temperature;
        qreal convectionCurrent = std::cos(m_frameCount * 0.001 + blob.phase) * 0.015;
        blob.velocity.rx() += thermalCurrent + convectionCurrent;

        // Reduced random drift for smoother movement
        blob.velocity.rx() += randomDouble(-horizontalDrift, horizontalDrift);

        // More realistic velocity limits
        blob.velocity.rx() = std::max(-0.25, std::min(0.25, blob.velocity.rx()));
        qreal maxVerticalVelocity = 1.2 * heightScale;
        blob.velocity.ry() = std::max(-maxVerticalVelocity, std::min(maxVerticalVelocity, blob.velocity.ry()));

        // Improved fluid damping
        blob.velocity *= fluidDamping;

        // Update position
        blob.position += blob.velocity;

        updateBlobDeformation(blob, m_frameCount * 0.001);

        // Smoother size variation
        blob.phase += blob.pulseSpeed;
        qreal temperatureExpansion = 0.94 + 0.12 * blob.temperature;
        qreal pulse = 0.98 + 0.04 * std::sin(blob.phase);
        blob.size = blob.baseSize * temperatureExpansion * pulse;

        // Improved state transitions
        qreal bottomZone = height() * 0.75;
        qreal topZone = height() * 0.25;

        if (blob.isRising && blob.position.y() < topZone && blob.temperature < 0.3) {
            blob.isRising = false;
            blob.density = randomDouble(1.1, 1.2);
            blob.velocity.ry() = std::abs(blob.velocity.ry()) * 0.3;
        } else if (!blob.isRising && blob.position.y() > bottomZone && blob.temperature > 0.7) {
            blob.isRising = true;
            blob.density = randomDouble(0.8, 0.9);
            blob.velocity.ry() = -std::abs(blob.velocity.ry()) * 0.3;
        }

        // Respawn logic
        qreal verticalMargin = height() * 0.2;
        bool needsRespawn = false;

        if (blob.isRising && (blob.position.y() < -verticalMargin || blob.lifetime > blob.maxLifetime)) {
            needsRespawn = true;
        } else if (!blob.isRising && (blob.position.y() > height() + verticalMargin || blob.lifetime > blob.maxLifetime)) {
            needsRespawn = true;
        }

        if (blob.position.x() < -width() * 0.6 || blob.position.x() > width() * 1.6) {
            needsRespawn = true;
        }

        if (needsRespawn) {
            respawnBlob(blob);
        }
    }

    // Improved blob interactions
    for (size_t i = 0; i < m_blobs.size(); ++i) {
        for (size_t j = i + 1; j < m_blobs.size(); ++j) {
            handleFluidBlobInteraction(m_blobs[i], m_blobs[j]);
        }
    }
}

void LavaLampPaintWidget::handleFluidBlobInteraction(LavaBlob& blob1, LavaBlob& blob2)
{
    qreal distance = QLineF(blob1.position, blob2.position).length();
    qreal combinedRadius = (blob1.size + blob2.size) * 0.6;

    if (distance < combinedRadius && distance > 0.1) {
        QPointF direction = (blob1.position - blob2.position) / distance;
        qreal overlap = combinedRadius - distance;
        qreal strength = overlap / combinedRadius;
        qreal scaledRepulsionStrength = 0.08 * m_scaleFactor;
        qreal scaledImpulseStrength = 0.06 * m_scaleFactor;

        // Smooth repulsion force
        qreal repulsionStrength = strength * 0.08;
        blob1.position += direction * scaledRepulsionStrength;
        blob2.position -= direction * scaledRepulsionStrength;

        // Fluid-like momentum exchange
        QPointF relativeVelocity = blob1.velocity - blob2.velocity;
        qreal velocityProjection = QPointF::dotProduct(relativeVelocity, direction);

        if (velocityProjection > 0) {
            qreal mass1 = blob1.size * blob1.density;
            qreal mass2 = blob2.size * blob2.density;
            qreal totalMass = mass1 + mass2;

            QPointF impulse = direction * velocityProjection * 0.06; // Reduced impulse for smoother interaction

            blob1.velocity -= impulse * (mass2 / totalMass);
            blob2.velocity += impulse * (mass1 / totalMass);
        }

        // Temperature and size mixing for more realistic fluid behavior
        if (strength > 0.4) {
            qreal tempAverage = (blob1.temperature + blob2.temperature) * 0.5;
            blob1.temperature += (tempAverage - blob1.temperature) * 0.05;
            blob2.temperature += (tempAverage - blob2.temperature) * 0.05;

            // Temporary size increase during interaction
            blob1.baseSize = blob1.originalSize * (1.0 + strength * 0.1);
            blob2.baseSize = blob2.originalSize * (1.0 + strength * 0.1);

            blob1.mergeTimer = 30;
            blob2.mergeTimer = 30;

            // Subtle deformation
            deformBlobFromCollision(blob1, blob2.position, strength * 0.5);
            deformBlobFromCollision(blob2, blob1.position, strength * 0.5);
        }
    }

    // Restore original sizes when not interacting
    if (blob1.mergeTimer == 0) {
        blob1.baseSize += (blob1.originalSize - blob1.baseSize) * 0.1;
    }
    if (blob2.mergeTimer == 0) {
        blob2.baseSize += (blob2.originalSize - blob2.baseSize) * 0.1;
    }
}

void LavaLampPaintWidget::updateBlobTemperature(LavaBlob& blob)
{
    // Calculate temperature based on current widget height, not fixed values
    qreal heightRatio = (height() - blob.position.y()) / height();

    // Clamp height ratio to handle bubbles outside visible area
    heightRatio = std::max(0.0, std::min(1.0, heightRatio));

    // Temperature varies from bottom (hot) to top (cold)
    qreal targetTemperature = heightRatio * 0.8 + 0.2;

    blob.temperature += (targetTemperature - blob.temperature) * blob.thermalConductivity;
    blob.temperature = std::max(0.05, std::min(1.0, blob.temperature));

    // Only change colors based on temperature if enabled (during break time)
    if (m_temperatureBasedColors) {
        if (blob.temperature > 0.7) {
            blob.color = m_primaryColor;
        } else if (blob.temperature > 0.3) {
            qreal ratio = (blob.temperature - 0.3) / 0.4;
            blob.color = QColor(
                static_cast<int>(m_secondaryColor.red() + (m_primaryColor.red() - m_secondaryColor.red()) * ratio),
                static_cast<int>(m_secondaryColor.green() + (m_primaryColor.green() - m_secondaryColor.green()) * ratio),
                static_cast<int>(m_secondaryColor.blue() + (m_primaryColor.blue() - m_secondaryColor.blue()) * ratio),
                static_cast<int>(m_secondaryColor.alpha() + (m_primaryColor.alpha() - m_secondaryColor.alpha()) * ratio)
            );
        } else {
            blob.color = m_secondaryColor;
        }
    }
    // If temperature-based colors are disabled, colors remain fixed as set initially
}

void LavaLampPaintWidget::updateBlobDeformation(LavaBlob& blob, qreal timeScale)
{
    // Restore circular shape more gradually for smoother appearance
    restoreCircularShape(blob);

    qreal velocityMagnitude = std::sqrt(blob.velocity.x() * blob.velocity.x() + blob.velocity.y() * blob.velocity.y());

    // Reduced deformation for more subtle effect
    if (velocityMagnitude > 0.02) {
        qreal deformStrength = std::min(velocityMagnitude * 0.2, 0.15);
        QPointF velocityDirection = blob.velocity / velocityMagnitude;

        for (int i = 0; i < blob.controlPoints.size(); ++i) {
            QPointF& point = blob.controlPoints[i];
            QPointF pointDir = point;
            qreal pointDist = QLineF(QPointF(0, 0), pointDir).length();

            if (pointDist > 0) {
                pointDir /= pointDist;
                qreal alignment = QPointF::dotProduct(pointDir, velocityDirection);

                if (alignment > 0) {
                    point += velocityDirection * deformStrength * blob.size * alignment * 0.8;
                } else {
                    point -= velocityDirection * deformStrength * blob.size * alignment * 0.15;
                }
            }
        }
    }

    // Subtle thermal effects
    qreal thermalDeform = (blob.temperature - 0.5) * 0.04;
    for (auto& point : blob.controlPoints) {
        point *= (1.0 + thermalDeform);
    }

    // Reduced organic wobbling for smoother appearance
    qreal wobbleStrength = 0.008 + blob.temperature * 0.005;
    for (int i = 0; i < blob.controlPoints.size(); ++i) {
        qreal wobblePhase = timeScale * 0.5 + blob.phase + i * 0.3;
        qreal wobble = std::sin(wobblePhase) * wobbleStrength;
        blob.controlPoints[i] *= (1.0 + wobble);
    }
}

void LavaLampPaintWidget::respawnBlob(LavaBlob& blob)
{
    qreal verticalMargin = height() * 0.15;

    if (blob.isRising) {
        blob.position = QPointF(
            randomDouble(-width() * 0.2, width() * 1.2),
            height() + randomDouble(verticalMargin * 0.2, verticalMargin)
        );
        blob.velocity = QPointF(
            randomDouble(-0.3, 0.3),
            randomDouble(-0.7, -0.4)
        );
        blob.density = randomDouble(0.75, 0.85);
        blob.temperature = randomDouble(0.85, 1.0);
    } else {
        blob.position = QPointF(
            randomDouble(-width() * 0.2, width() * 1.2),
            randomDouble(-verticalMargin, -verticalMargin * 0.2)
        );
        blob.velocity = QPointF(
            randomDouble(-0.3, 0.3),
            randomDouble(0.4, 0.7)
        );
        blob.density = randomDouble(1.15, 1.25);
        blob.temperature = randomDouble(0.1, 0.3);
    }

    blob.lifetime = 0;
    blob.maxLifetime = QRandomGenerator::global()->bounded(500, 1000);

    // Use responsive size ranges for respawned blobs
    blob.baseSize = randomDouble(m_baseMinSize, m_baseMaxSize);
    blob.originalSize = blob.baseSize;
    blob.mergeTimer = 0;

    static int colorIndex = 0;
    blob.color = (colorIndex % 2 == 0) ? m_primaryColor : m_secondaryColor;
    colorIndex++;

    initializeBlobShape(blob);
}

void LavaLampPaintWidget::handleAdvancedBlobInteraction(LavaBlob& blob1, LavaBlob& blob2)
{
    qreal distance = QLineF(blob1.position, blob2.position).length();
    qreal interactionDistance = (blob1.size + blob2.size) * 0.45;

    if (distance < interactionDistance && distance > 0) {
        qreal strength = 1.0 - (distance / interactionDistance);
        QPointF direction = (blob1.position - blob2.position) / distance;

        if (strength > 0.6) {
            deformBlobFromCollision(blob1, blob2.position, strength);
            deformBlobFromCollision(blob2, blob1.position, strength);

            // Temperature mixing
            qreal tempMix = (blob1.temperature + blob2.temperature) * 0.5;
            blob1.temperature += (tempMix - blob1.temperature) * 0.08;
            blob2.temperature += (tempMix - blob2.temperature) * 0.08;

            blob1.baseSize = blob1.originalSize * (1.0 + strength * 0.15);
            blob2.baseSize = blob2.originalSize * (1.0 + strength * 0.15);

            blob1.mergeTimer = 60;
            blob2.mergeTimer = 60;
        }

        // Gentle collision response
        qreal overlap = interactionDistance - distance;
        blob1.position += direction * overlap * 0.15;
        blob2.position -= direction * overlap * 0.15;

        // Momentum transfer with vertical bias
        QPointF relativeVelocity = blob1.velocity - blob2.velocity;
        qreal velocityComponent = QPointF::dotProduct(relativeVelocity, direction);

        if (velocityComponent > 0) {
            qreal massRatio = blob1.density / (blob1.density + blob2.density);
            QPointF impulse = direction * velocityComponent * 0.1;

            // Reduce horizontal impulse to keep vertical movement
            impulse.setX(impulse.x() * 0.3);

            blob1.velocity -= impulse * (1.0 - massRatio);
            blob2.velocity += impulse * massRatio;
        }
    }

    if (blob1.mergeTimer == 0) {
        blob1.baseSize = blob1.originalSize;
    }
    if (blob2.mergeTimer == 0) {
        blob2.baseSize = blob2.originalSize;
    }
}

void LavaLampPaintWidget::restoreCircularShape(LavaBlob& blob)
{
    const int numPoints = blob.controlPoints.size();
    for (int i = 0; i < numPoints; ++i) {
        qreal angle = (2.0 * M_PI * i) / numPoints;
        QPointF targetPoint(
            std::cos(angle) * blob.size,
            std::sin(angle) * blob.size
        );

        // Increased surface tension for more stable shapes
        blob.controlPoints[i] += (targetPoint - blob.controlPoints[i]) * (blob.surfaceTension * 1.5);
    }
}

void LavaLampPaintWidget::deformBlobFromCollision(LavaBlob& blob, const QPointF& collisionPoint, qreal strength)
{
    QPointF collisionDirection = collisionPoint - blob.position;
    qreal collisionDistance = QLineF(QPointF(0, 0), collisionDirection).length();

    if (collisionDistance > 0) {
        collisionDirection /= collisionDistance;

        for (int i = 0; i < blob.controlPoints.size(); ++i) {
            QPointF& point = blob.controlPoints[i];
            QPointF pointDirection = point;
            qreal pointDistance = QLineF(QPointF(0, 0), pointDirection).length();

            if (pointDistance > 0) {
                pointDirection /= pointDistance;
                qreal alignment = QPointF::dotProduct(pointDirection, collisionDirection);

                if (alignment > 0) {
                    qreal deformAmount = strength * alignment * 0.3;
                    point -= collisionDirection * deformAmount * blob.size;
                } else {
                    qreal deformAmount = strength * std::abs(alignment) * 0.1;
                    point += collisionDirection * deformAmount * blob.size;
                }
            }
        }
    }
}

void LavaLampPaintWidget::updateThermalDynamics()
{
    m_heatSourceIntensity = 0.85 + 0.15 * std::sin(m_frameCount * 0.0008);
    m_temperature = 0.4 + 0.4 * std::sin(m_frameCount * 0.0012);
}

void LavaLampPaintWidget::createLavaLampBackground(QPainter& painter)
{
    QLinearGradient baseGradient(0, 0, 0, height());

    QColor topColor = m_backgroundColor;
    topColor.setBlue(std::min(255, static_cast<int>(topColor.blue() + 25)));
    topColor.setAlpha(40);

    QColor midColor = m_backgroundColor;
    midColor.setAlpha(65);

    QColor bottomColor = m_backgroundColor;
    bottomColor.setRed(std::min(255, static_cast<int>(bottomColor.red() + 30 * m_heatSourceIntensity)));
    bottomColor.setGreen(std::min(255, static_cast<int>(bottomColor.green() + 15 * m_heatSourceIntensity)));
    bottomColor.setAlpha(90);

    baseGradient.setColorAt(0.0, topColor);
    baseGradient.setColorAt(0.5, midColor);
    baseGradient.setColorAt(1.0, bottomColor);

    painter.fillRect(rect(), baseGradient);
}

void LavaLampPaintWidget::drawThermalEffects(QPainter& painter)
{
    painter.setCompositionMode(QPainter::CompositionMode_Screen);

    // Scale heat effect based on widget height
    qreal heatHeight = height() * 0.18;
    QRadialGradient heatGradient(width() * 0.5, height(), width() * 0.4);

    QColor heatCenter(255, 80, 20, static_cast<int>(25 * m_heatSourceIntensity));
    QColor heatEdge(255, 40, 0, static_cast<int>(8 * m_heatSourceIntensity));

    heatGradient.setColorAt(0.0, heatCenter);
    heatGradient.setColorAt(0.7, heatEdge);
    heatGradient.setColorAt(1.0, QColor(0, 0, 0, 0));

    painter.fillRect(0, height() - heatHeight, width(), heatHeight, heatGradient);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
}

void LavaLampPaintWidget::drawBlobGlows(QPainter& painter)
{
    painter.setCompositionMode(QPainter::CompositionMode_Screen);

    for (const auto& blob : m_blobs) {
        if (blob.temperature > 0.5) {
            QRadialGradient glowGradient(blob.position, blob.size * 1.5);

            QColor glowCenter = blob.color;
            glowCenter.setAlphaF(0.12 * blob.temperature);

            QColor glowEdge = blob.color;
            glowEdge.setAlphaF(0.02 * blob.temperature);

            glowGradient.setColorAt(0.0, glowCenter);
            glowGradient.setColorAt(0.6, glowEdge);
            glowGradient.setColorAt(1.0, QColor(0, 0, 0, 0));

            painter.setBrush(QBrush(glowGradient));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(blob.position, blob.size * 1.5, blob.size * 1.5);
        }
    }

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
}


void LavaLampPaintWidget::drawLiquidBlobs(QPainter& painter)
{
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    for (const auto& blob : m_blobs) {
        QPainterPath path = createBlobPath(blob);

        QRadialGradient gradient(blob.position, blob.size * 0.9);

        QColor centerColor = blob.color;
        QColor midColor = blob.color;
        QColor edgeColor = blob.color;

        // Use consistent alpha values - remove temperature-based transparency
        centerColor.setAlphaF(blob.alpha);
        midColor.setAlphaF(blob.alpha * 0.8);
        edgeColor.setAlphaF(blob.alpha * 0.4);

        // Keep the temperature-based brightness effect but not transparency
        if (blob.temperature > 0.7) {
            centerColor = centerColor.lighter(105 + static_cast<int>(15 * blob.temperature));
        }

        gradient.setColorAt(0.0, centerColor);
        gradient.setColorAt(0.3, centerColor);
        gradient.setColorAt(0.7, midColor);
        gradient.setColorAt(0.95, edgeColor);
        gradient.setColorAt(1.0, QColor(blob.color.red(), blob.color.green(), blob.color.blue(), 0));

        painter.setBrush(QBrush(gradient));
        painter.setPen(Qt::NoPen);
        painter.fillPath(path, QBrush(gradient));
    }
}

void LavaLampPaintWidget::drawBlobHighlights(QPainter& painter)
{
    painter.setCompositionMode(QPainter::CompositionMode_Overlay);

    for (const auto& blob : m_blobs) {
        if (blob.temperature > 0.6) {
            QPointF highlightPos = blob.position + QPointF(-blob.size * 0.2, -blob.size * 0.25);
            QRadialGradient highlight(highlightPos, blob.size * 0.3);

            QColor highlightColor(255, 255, 255, static_cast<int>(25 * blob.temperature));
            highlight.setColorAt(0.0, highlightColor);
            highlight.setColorAt(1.0, QColor(255, 255, 255, 0));

            painter.setBrush(QBrush(highlight));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(highlightPos, blob.size * 0.3, blob.size * 0.3);
        }
    }

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
}

QPainterPath LavaLampPaintWidget::createBlobPath(const LavaBlob& blob) const
{
    QPainterPath path;

    if (blob.controlPoints.size() < 3) {
        path.addEllipse(blob.position, blob.size, blob.size);
        return path;
    }

    QVector<QPointF> worldPoints;
    for (const auto& point : blob.controlPoints) {
        worldPoints.append(blob.position + point);
    }

    path.moveTo(worldPoints[0]);

    for (int i = 0; i < worldPoints.size(); ++i) {
        int next = (i + 1) % worldPoints.size();
        int prev = (i - 1 + worldPoints.size()) % worldPoints.size();
        int nextNext = (i + 2) % worldPoints.size();

        QPointF control1 = worldPoints[i] + (worldPoints[next] - worldPoints[prev]) * 0.15;
        QPointF control2 = worldPoints[next] + (worldPoints[i] - worldPoints[nextNext]) * 0.15;

        path.cubicTo(control1, control2, worldPoints[next]);
    }

    path.closeSubpath();
    return path;
}

void LavaLampPaintWidget::drawGlassEffect(QPainter& painter)
{
    painter.setCompositionMode(QPainter::CompositionMode_Overlay);

    QLinearGradient glassGradient(0, 0, width(), 0);
    glassGradient.setColorAt(0.0, QColor(255, 255, 255, 0));
    glassGradient.setColorAt(0.08, QColor(255, 255, 255, 0));
    glassGradient.setColorAt(0.12, QColor(255, 255, 255, 20));
    glassGradient.setColorAt(0.18, QColor(255, 255, 255, 8));
    glassGradient.setColorAt(0.25, QColor(255, 255, 255, 0));
    glassGradient.setColorAt(1.0, QColor(255, 255, 255, 0));

    painter.fillRect(rect(), glassGradient);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
}