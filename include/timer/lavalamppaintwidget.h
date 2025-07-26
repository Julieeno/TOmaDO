#ifndef LAVALAMPPAINTWIDGET_H
#define LAVALAMPPAINTWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QPainter>
#include <QRectF>
#include <QColor>
#include <QPainterPath>
#include <vector>

class LavaLampPaintWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LavaLampPaintWidget(QWidget* parent = nullptr);
    ~LavaLampPaintWidget() override = default;

    void startAnimation();

    void stopAnimation();

    void setColorScheme(const QColor& primaryColor, const QColor& secondaryColor, const QColor& backgroundColor);

    void setBorderRadius(int radius);

    void setTemperatureBasedColors(bool enabled);

protected:
    void paintEvent(QPaintEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateAnimation();

private:
    struct LavaBlob {
        QPointF position;
        QPointF velocity;
        qreal size;
        qreal baseSize;
        qreal originalSize;
        QColor color;
        qreal alpha;
        qreal phase;
        qreal pulseSpeed;
        qreal temperaturePhase;
        qreal density;
        qreal temperature;
        qreal viscosity;
        qreal thermalConductivity;
        int lifetime;
        int maxLifetime;
        bool isRising;
        int mergeTimer;
        qreal surfaceTension;

        // Control points for organic shape
        std::vector<QPointF> controlPoints;
    };

    void initializeBlobs();
    void initializeBlobShape(LavaBlob& blob);
    void updateBlobs();
    void updateBlobTemperature(LavaBlob& blob);
    void updateBlobDeformation(LavaBlob& blob, qreal timeScale);
    void respawnBlob(LavaBlob& blob);
    void restoreCircularShape(LavaBlob& blob);
    void handleAdvancedBlobInteraction(LavaBlob& blob1, LavaBlob& blob2);
    void deformBlobFromCollision(LavaBlob& blob, const QPointF& collisionPoint, qreal strength);
    void updateThermalDynamics();
    void createLavaLampBackground(QPainter& painter);
    void drawThermalEffects(QPainter& painter);

    void drawBlobGlows(QPainter &painter);

    void drawLiquidBlobs(QPainter& painter);

    void drawBlobHighlights(QPainter &painter);

    void drawGlassEffect(QPainter& painter);
    QPainterPath createBlobPath(const LavaBlob& blob) const;

    void handleFluidBlobInteraction(LavaBlob& blob1, LavaBlob& blob2);

    qreal randomDouble(qreal min, qreal max);

    QTimer* m_animationTimer;
    std::vector<LavaBlob> m_blobs;

    QColor m_primaryColor;
    QColor m_secondaryColor;
    QColor m_backgroundColor;

    int m_frameCount;
    int m_borderRadius;
    qreal m_temperature;
    qreal m_heatSourceIntensity;
    bool m_temperatureBasedColors;

    qreal m_scaleFactor;
    qreal m_baseMinSize;
    qreal m_baseMaxSize;

    // Add this helper method
    void calculateScaleFactor();

    static constexpr int BLOB_COUNT = 20;
    static constexpr int ANIMATION_INTERVAL = 60;
    static constexpr qreal SPEED_FACTOR = 0.8;
    static constexpr qreal BASE_MIN_BLOB_SIZE = 30.0;
    static constexpr qreal BASE_MAX_BLOB_SIZE = 100.0;
    static constexpr qreal MIN_SCALE_FACTOR = 0.5;
    static constexpr qreal MAX_SCALE_FACTOR = 2.0;
};

#endif // LAVALAMPPAINTWIDGET_H