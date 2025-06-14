#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// Points of interest for zooming
struct ZoomPoint
{
    double      x;
    double      y;
    double      zoomStart;
    double      zoomEnd;
    double      zoomFactor;
    std::string name;

    ZoomPoint(double             _x,
              double             _y,
              double             _zoomStart,
              double             _zoomEnd,
              double             _zoomFactor,
              const std::string& _name)
        : x(_x),
          y(_y),
          zoomStart(_zoomStart),
          zoomEnd(_zoomEnd),
          zoomFactor(_zoomFactor),
          name(_name)
    {
    }
};

int main()
{
    // Define interesting points for zooming that show detail at high zoom levels
    std::vector<ZoomPoint> zoomPoints = {
        // Main view for reference
        ZoomPoint(0.0, 0.0, 1.0, 1.0, 1.0, "overview"),

        // Dendrite area with fractal branching (good for zooming)
        ZoomPoint(-0.743643887037151, 0.131825904205330, 10.0, 10.0, 1.0, "dendrite_low"),
        ZoomPoint(-0.743643887037151, 0.131825904205330, 100.0, 100.0, 1.0, "dendrite_med"),
        ZoomPoint(-0.743643887037151, 0.131825904205330, 1000.0, 1000.0, 1.0, "dendrite_high"),
        ZoomPoint(-0.743643887037151, 0.131825904205330, 10000.0, 10000.0, 1.0, "dendrite_extreme"),

        // Filament point with intricate detail
        ZoomPoint(0.324957181995826, 0.428847248551299, 50.0, 50.0, 1.0, "filament_low"),
        ZoomPoint(0.324957181995826, 0.428847248551299, 500.0, 500.0, 1.0, "filament_med"),
        ZoomPoint(0.324957181995826, 0.428847248551299, 5000.0, 5000.0, 1.0, "filament_high"),

        // Spiral point showing rotation with detail
        ZoomPoint(-0.17500193791798, 0.65906293634335, 200.0, 200.0, 1.0, "spiral_low"),
        ZoomPoint(-0.17500193791798, 0.65906293634335, 2000.0, 2000.0, 1.0, "spiral_high"),

        // Mini-brot area with self-similar structures
        ZoomPoint(-0.5637347919485, -0.6435369904424, 75.0, 75.0, 1.0, "minibrot_low"),
        ZoomPoint(-0.5637347919485, -0.6435369904424, 750.0, 750.0, 1.0, "minibrot_high")};

    // Julia set parameter c = -0.8 + 0.156i
    double cReal = -0.7;
    double cImag = 0.27015;  // Different parameter to show more intricate detail

    // Generate comparison images at each zoom level
    for (const auto& point : zoomPoints)
    {
        std::cout << "Generating comparison for " << point.name << " at zoom level "
                  << point.zoomStart << std::endl;

        // Generate posit version
        std::string posit_cmd = "./julia_set_posit " + std::to_string(point.zoomStart) + " " +
                                std::to_string(point.x) + " " + std::to_string(point.y) + " " +
                                std::to_string(cReal) + " " + std::to_string(cImag);

        // Generate float version
        std::string float_cmd = "./julia_set_float " + std::to_string(point.zoomStart) + " " +
                                std::to_string(point.x) + " " + std::to_string(point.y) + " " +
                                std::to_string(cReal) + " " + std::to_string(cImag);

        std::cout << "Running: " << posit_cmd << std::endl;
        system(posit_cmd.c_str());

        std::cout << "Running: " << float_cmd << std::endl;
        system(float_cmd.c_str());
    }

    std::cout << "\nAll comparison images generated successfully." << std::endl;
    std::cout << "To view the difference between posit and float precision, compare:" << std::endl;
    std::cout << "  - julia_posit_zoom_*.ppm (posit32 version)" << std::endl;
    std::cout << "  - julia_float_zoom_*.ppm (double precision float version)" << std::endl;
    std::cout << "\nThe most dramatic differences will be visible at extreme zoom levels (1000+)"
              << std::endl;
    std::cout << "where float precision issues become apparent but posits maintain higher accuracy."
              << std::endl;
    std::cout << "\nTry viewing the dendrite_extreme and filament_high images to see the most "
                 "striking differences."
              << std::endl;

    return 0;
}