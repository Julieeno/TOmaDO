#ifndef COLORSERVICE_H
#define COLORSERVICE_H

#include <QString>
#include <QStringList>

class ColorService {
public:
    static ColorService& instance();
    
    QString getColorName(const QString& colorCode) const;
    QStringList getAvailableColors() const;
    bool isValidColor(const QString& colorCode) const;
    
private:
    ColorService() = default;
    
    // Prevent copying
    ColorService(const ColorService&) = delete;
    ColorService& operator=(const ColorService&) = delete;
};

#endif // COLORSERVICE_H