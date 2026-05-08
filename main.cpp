#include <iostream>
#include <fstream>
#include <format>
#include <vector>
#include <math.h>
#include <cstring>
using namespace std;

struct circle
{
    int cx;
    int cy;
    int r;
    string fill;
    string stroke;
    int strokeWidth;
};

struct line
{
    int x1;
    int y1;
    int x2;
    int y2;
    string stroke;
    int strokeWidth;
};

bool checkForHelpFlag(char** argv)
{
    if (strcmp(argv[1], "-h") != 0)
    {
        return false;
    }
    cout << "########\n"
            "# Help #\n"
            "########\n\n"
            "-h - show this list\n\n"
            "# Correct Usage\n"
            "graphgen.exe <OUTPUT_PATH> <WIDTH> <HEIGHT> <MARGIN> <RADIUS> <LAYER_SPACING> <STROKE_WIDTH> <STROKE> <FILL> [LAYER_SIZE...]\n"
            "\n";
    return true;
}

void doneMsg()
{
    cout << "Done.\n";
}

// Source - https://stackoverflow.com/a/2845275
// Posted by paercebal, modified by community. See post 'Timeline' for change history
// Retrieved 2026-05-07, License - CC BY-SA 4.0

inline bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}

int main(int argc, char** argv)
{
    cout << "SVG Graph Visualizer version 0.0.1\n";
    cout << argc << " arguments\n";

    if (argc < 2)
    {
        cout << "Missing arguments. Try -h for help\n";
        return -1;
    }
    if (checkForHelpFlag(argv))
    {
        return 0;
    }
    else
    {
        cout << "-h for help\n\n";
    }

    if (argc < 8)
    {
        cout << "Missing arguments. Try -h for help [line 84]\n";
        return -1;
    }

    const char* PATH = argv[1] ? argv[1] : "generated/graph.svg";
    const int WIDTH = isInteger(argv[2]) ? stoi(argv[2]) : 600;
    const int HEIGHT = isInteger(argv[3]) ? stoi(argv[3]) : 800;
    const int MARGIN = isInteger(argv[4]) ? stoi(argv[4]) : 32;
    const int RADIUS = isInteger(argv[5]) ? stoi(argv[5]) : 16;
    const int LAYER_SPACING = isInteger(argv[6]) ? stoi(argv[6]) : 64;
    const int STROKE_WIDTH = isInteger(argv[7]) ? stoi(argv[7]) : 2;
    const string STROKE = argv[8] ? argv[8] : "#fff";
    const string FILL = argv[9] ? argv[9] : "#000";

    for (size_t i = 10; i < static_cast<size_t>(argc); i++)
    {
        if (!isInteger(argv[i]))
        {
            cout << "Layer sizes must be integer values.\n";
            return -1;
        }
    }
    
    const int LAYER_SIZES_SIZE = argc - 10;
    vector<int> layerSizes(LAYER_SIZES_SIZE);
    for (size_t i = 0; i < static_cast<size_t>(LAYER_SIZES_SIZE); i++)
    {
        layerSizes[i] = stoi(argv[i + 10]);
    }

    ofstream out(PATH);
    if (out.fail())
    {
        cout << "Failed to open file\n";
        return -1;
    }
    cout << PATH << " opened successfully\n";

    // int layerSizes[] = 
    // {
    //     12, 8, 8, 4
    // };

    vector<vector<circle>> neurons;
    vector<line> lines;

    // Calculation
    cout << "Calculating circles and lines.\n";
    // Circle calculation pass
    cout << "Pass 0: Circles...\n";

    size_t greatestLayerIndex = 0;
    for (size_t i = 0; i < layerSizes.size(); i++)
    {
        greatestLayerIndex = layerSizes[i] > layerSizes[greatestLayerIndex] ? i : greatestLayerIndex;
    }
    int greatestLayerSize = layerSizes[greatestLayerIndex];

    int circleMarginSize = 2 * RADIUS + MARGIN + STROKE_WIDTH;

    size_t layerSizesSize = layerSizes.size();

    for (size_t l = 0; l < layerSizesSize; l++)
    {
        neurons.push_back({});
        int layerSubmaxOffset = circleMarginSize * (greatestLayerSize - layerSizes[l]) / 2;
        for (size_t n = 0; n < static_cast<size_t>(layerSizes[l]); n++)
        {
            int cx, cy;

            cx = RADIUS + MARGIN + STROKE_WIDTH / 2 + l * (2 * RADIUS + LAYER_SPACING + STROKE_WIDTH);
            cy = RADIUS + MARGIN + STROKE_WIDTH / 2 + n * (2 * RADIUS + MARGIN + STROKE_WIDTH) + layerSubmaxOffset;
            
            neurons[l].push_back({cx, cy, RADIUS, FILL, STROKE, STROKE_WIDTH});
        }
    }
    doneMsg();

    // Line calculation pass
    cout << "Pass 1: Lines...\n";
    
    for (size_t l = 0; l < neurons.size() - 1; l++)
    {
        for (size_t n = 0; n < neurons[l].size(); n++)
        {
            circle currNeur = neurons[l][n];
            int x1 = currNeur.cx;
            int y1 = currNeur.cy;

            for (size_t n_next = 0; n_next < neurons[l + 1].size(); n_next++)
            {
                circle nextLayerNeur = neurons[l + 1][n_next];
                int x2 = nextLayerNeur.cx;
                int y2 = nextLayerNeur.cy;

                lines.push_back({x1, y1, x2, y2, STROKE, STROKE_WIDTH});
            }
        }
    }
    doneMsg();

    // SVG generation
    cout << "Generating SVG.\n";
    string svgContents;
    
    svgContents += format("<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"{}\" height=\"{}\">\n",
                            WIDTH, HEIGHT);
                            
    // Line pass
    cout << "Pass 0: Generating lines...\n";
    for (size_t i = 0; i < lines.size(); i++)
    {
        line cl = lines[i];

        svgContents += format("<line x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\" stroke=\"{}\" stroke-width=\"{}\"/>\n",
                                cl.x1, cl.y1, cl.x2, cl.y2, STROKE, STROKE_WIDTH);
    }
    doneMsg();

    // Circle pass
    cout << "Pass 1: Generating circles...\n";
    for (size_t l = 0; l < layerSizes.size(); l++)
    {
        for (size_t n = 0; n < static_cast<size_t>(layerSizes[l]); n++)
        {
            circle cn = neurons[l][n];

            svgContents += format("<circle cx=\"{}\" cy=\"{}\" r=\"{}\" fill=\"{}\" stroke=\"{}\" stroke-width=\"{}\"/>\n",
                                        cn.cx, cn.cy, cn.r, cn.fill, STROKE, STROKE_WIDTH);
        }
    }
    doneMsg();

    svgContents += R"LABEL(</svg>)LABEL";

    out << svgContents;
    out.close();
    return 0;
}