#ifndef COLOUR_H
#define COLOUR_H

class Colour {
    double red, green, blue, alpha;

public:
    Colour(double _red = 0, double _green = 0, double _blue = 0, double _alpha = 1)
        : red(_red), green(_green), blue(_blue), alpha(_alpha) {}

    double getRed() const { return red; }
    double getGreen() const { return green; }
    double getBlue() const { return blue; }
    double getAlpha() const { return alpha; }

    void setRed(double redValue) { red = redValue; }
    void setGreen(double greenValue) { green = greenValue; }
    void setBlue(double blueValue) { blue = blueValue; }
    void setAlpha(double alphaValue) { alpha = alphaValue; }

    // Multiply the colour by a scalar value
    Colour operator*(double scalar) const {
        double newRed = red * scalar;
        double newGreen = green * scalar;
        double newBlue = blue * scalar;
        double newAlpha = alpha * scalar;
        return Colour(newRed, newGreen, newBlue, newAlpha);
    }

    // Multiply two colours component-wise
    Colour operator*(const Colour& other) const {
        double newRed = red * other.red;
        double newGreen = green * other.green;
        double newBlue = blue * other.blue;
        double newAlpha = alpha * other.alpha;
        return Colour(newRed, newGreen, newBlue, newAlpha);
    }

    // Add two colours
    Colour operator+(const Colour& other) const {
        double newRed = red + other.red;
        double newGreen = green + other.green;
        double newBlue = blue + other.blue;
        double newAlpha = alpha + other.alpha;
        return Colour(newRed, newGreen, newBlue, newAlpha);
    }

    // Subtract two colors
    Colour operator-(const Colour& other) const {
        double newRed = red - other.red;
        double newGreen = green - other.green;
        double newBlue = blue - other.blue;
        double newAlpha = alpha - other.alpha;
        return Colour(newRed, newGreen, newBlue, newAlpha);
    }

    // Divide the colour by a scalar value
    Colour operator/(double scalar) const {
        if (scalar != 0.0) {
            double invScalar = 1.0 / scalar;
            double newRed = red * invScalar;
            double newGreen = green * invScalar;
            double newBlue = blue * invScalar;
            double newAlpha = alpha * invScalar;
            return Colour(newRed, newGreen, newBlue, newAlpha);
        }
        // Division by zero, return a black colour
        return Colour();
    }

    // Add two colors and update the current colour
    Colour& operator+=(const Colour& other) {
        red += other.red;
        green += other.green;
        blue += other.blue;
        alpha += other.alpha;
        return *this;
    }

    // Multiply the color by another colour and update the current color
    Colour& operator*=(const Colour& other) {
        red *= other.red;
        green *= other.green;
        blue *= other.blue;
        alpha *= other.alpha;
        return *this;
    }

    // Add two colors and update the current colour
    void blendAdd(const Colour& other) {
        red += other.red;
        green += other.green;
        blue += other.blue;
        alpha += other.getAlpha();
    }

    // Multiply the color by another colour and update the current color
    void blendMultiply(const Colour& other) {
        red *= other.red;
        green *= other.green;
        blue *= other.blue;
        alpha *= other.alpha;
    }
};

#endif
