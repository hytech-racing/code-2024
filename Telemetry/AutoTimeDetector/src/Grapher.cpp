#include <thread>
#include "Grapher.hpp"
#include "pbPlots.hpp"
#include "supportLib.hpp"

void Grapher::graph(const std::vector<Point> &positions, int begin_idx, math::Circle c, double total_delta) {
    bool success;
    StringReference *errorMessage = CreateStringReferenceLengthValue(0, L' ');
    RGBABitmapImageReference *imageReference = CreateRGBABitmapImageReference();

    std::vector<double> computed_xs;
    std::vector<double> computed_ys;
    std::vector<double> other_xs;
    std::vector<double> other_ys;

    for(int i = 0; i < int(positions.size()); i++){
        if(i >= begin_idx){
            computed_xs.push_back(positions[i].x);
            computed_ys.push_back(positions[i].y);
        }else{
            other_xs.push_back(positions[i].x);
            other_ys.push_back(positions[i].y);
        }
    }

    ScatterPlotSeries *series = GetDefaultScatterPlotSeriesSettings();
    series->xs = &computed_xs;
    series->ys = &computed_ys;
    series->linearInterpolation = false;
    series->pointType = toVector(L"dots");
    series->color = CreateRGBColor(1, 0, 0);

    ScatterPlotSeries *series2 = GetDefaultScatterPlotSeriesSettings();
    series2->xs = &other_xs;
    series2->ys = &other_ys;
    series2->linearInterpolation = false;
    series2->pointType = toVector(L"dots");
    series2->lineThickness = 2;
    series2->color = CreateRGBColor(0, 0, 1);

    std::vector<double> circle_x;
    std::vector<double> circle_y;

    for(double i = 0; i < 2*M_PI+0.1; i+=0.1){
        circle_x.push_back(c.x + c.radius*std::cos(i));
        circle_y.push_back(c.y + c.radius*std::sin(i));
    }

    ScatterPlotSeries *series3 = GetDefaultScatterPlotSeriesSettings();
    series3->xs = &circle_x;
    series3->ys = &circle_y;
    series3->linearInterpolation = true;
    series3->lineType = toVector(L"solid");
    series3->color = CreateRGBColor(1, 0, 0);

    std::string title = std::to_string(total_delta);
    std::wstring wtitle = std::wstring(title.begin(), title.end());

    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
    settings->width = 600;
    settings->height = 600;
    settings->autoBoundaries = true;
    settings->autoPadding = true;
    settings->title = toVector(wtitle.c_str());
    settings->xLabel = toVector(L"");
    settings->yLabel = toVector(L"");
    if(!computed_xs.empty()) settings->scatterPlotSeries->push_back(series);
    if(!other_xs.empty()) settings->scatterPlotSeries->push_back(series2);
    settings->scatterPlotSeries->push_back(series3);

    success = DrawScatterPlotFromSettings(imageReference, settings, errorMessage);

    if(success){
        std::vector<double> *pngdata = ConvertToPNG(imageReference->image);
        WriteToFile(pngdata, get_image_name());
        DeleteImage(imageReference->image);
    }else{
        std::cerr << "Error: ";
        for(wchar_t c : *errorMessage->string){
            std::cerr << c;
        }
        std::cerr << std::endl;
    }
}

Grapher::Grapher() {

}

std::string Grapher::get_image_name() {
    std::string ret = std::to_string(cnt++);
    while(int(ret.size()) < 4) ret = "0" + ret;
    return "../graphs/graph_" + ret + ".png";
}
