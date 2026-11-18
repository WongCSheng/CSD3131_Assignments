//#include "../shared/energy_map.h"
#include "../lib/energy.h"

// https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/
// https://github.com/arrij46/Gradient-Edge-Detection/blob/main/Serialq1.cpp

//References for time
//https://dilipkumar.medium.com/c-chrono-for-time-71b73d9e33f6
//https://en.cppreference.com/cpp/chrono/duration
//https://pyimagesearch.com/2017/01/23/seam-carving-with-opencv-python-and-scikit-image/ - to reference the image output of the energy and seams


#include <iostream>
#include <tuple>
#include <queue>
#include <climits>
#include <algorithm>
#include <chrono>
#include <cmath>

//type declarations
using EnergyRow = std::vector<int>;
using EnergyMap = std::vector<EnergyRow>;

// y/x convention
EnergyMap buildEnergyMap(const imgData& imd, int& max)
{
    int w = imd.getWidth();
    int h = imd.getHeight();

    EnergyMap em = EnergyMap(h, std::vector<int>(w));

    for (int i=0; i < h; i++)
    {
        for (int j=0; j < w; j++)
        {
            em[i][j] = generateEnergyValue(imd, j,i);
            //Get the max energy value
            if (max < std::abs(em[i][j])) {
                max = std::abs(em[i][j]);
            }

        }
    }

    return em;
}

void energyMapOutput(const imgData& imd, int max, const EnergyMap& em, const std::string& path, imgData& gradient) {
    int w = imd.getWidth();
    int h = imd.getHeight();

    std::vector<std::vector<double>> norm(h, std::vector<double>(w));
    //norm.reserve(h);

    for (int i = 0; i < h; i++)
    {
        norm[i].reserve(w);
        for (int j = 0; j < w; j++)
        {
            norm[i][j] =(std::fabs(static_cast<double>(em[i][j])) / static_cast<double>(max));
            //pow is needed for gamma correction (to make the faint edges more obvious)
            norm[i][j] = std::pow(norm[i][j], 0.3);
            //Multiply by 255 (0 - 255) color
            norm[i][j] *= 255.0;
            
        }
    
    }
    //Create the data for imgdata with the gradient values
    std::vector<unsigned char> gradient_data;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            //Get the value
            int val = norm[i][j];
            //clamp it between 0 to 255
            if (val > 255) {
                val = 255;
            }
            else if (val < 0) {
                val = 0;
            }
            //Set rgb to value for greyscale
            gradient_data.push_back(val);
            gradient_data.push_back(val);
            gradient_data.push_back(val);
            //Make alpha value to be 255 no matter
            if (imd.getCh() == 4) { gradient_data.push_back(255); } // alpha channel is stored 0-255 like the rest
        }
    }
    //Create an imgdata
    //imgData gradient;
    //Set the values based on imd and the gradient data
    gradient.createMap(imd.getWidth(), imd.getHeight(), imd.getCh(), gradient_data);
    //Write out as image
    gradient.writeImg(path);


}

void seamOutput(const imgData& imd, const std::string& path, std::vector<std::vector<int>>& allseams) {
    int w = imd.getWidth();
    int h = imd.getHeight();

    std::vector<unsigned char> greyscale_data = imd.getData();

    std::vector<unsigned char> gradient_data;

    for (int i = 0; i < h; i++)
    {
        std::vector<int> seampos = allseams[i]; // get the seam col data
        for (int j = 0; j < w; j++)
        {
            //if value is not inside the vector, just get the greyscale value as it is not a seam
            if (std::find(seampos.begin(), seampos.end(), j) == seampos.end()) {
                size_t pos = (i * w + j) * imd.getCh();
                gradient_data.push_back(greyscale_data[pos]);
                gradient_data.push_back(greyscale_data[pos +1]);
                gradient_data.push_back(greyscale_data[pos +2]);
                
                if (imd.getCh() == 4) { gradient_data.push_back(greyscale_data[pos+3]); } 
                continue;
            }
            
            //Set rgb to value for greyscale
            gradient_data.push_back(255);
            gradient_data.push_back(0);
            gradient_data.push_back(0);
            //Make alpha value to be 255 no matter
            if (imd.getCh() == 4) { gradient_data.push_back(255); } // alpha channel is stored 0-255 like the rest
        }
    }
    //Create an imgdata
    imgData gradient;
    //Set the values based on imd and the gradient data
    gradient.createMap(imd.getWidth(), imd.getHeight(), imd.getCh(), gradient_data);
    //Write out as image
    gradient.writeImg(path);
}

//Add the current seams to the combined total seams
void addSeamsToAll(std::vector<std::vector<int>>& allseams, int h, std::vector<int>& seams) {
    for (int i = 0; i < h; i++) {
        allseams[i].push_back(seams[i]);
    }
}
//Get the original indexes with the help of origin map
std::vector<int> getOriginal(std::vector<int>& seams, int w, int h, std::vector<std::vector<int>>& origin_map) {
    std::vector<int> tmpVec;
    for (int i = 0; i < h; i++) {
        int c = seams[i];
        int originalc = origin_map[i][c];
        tmpVec.push_back(originalc);
    }
    return tmpVec;
}

//Shrink the origin map to match the removal of seams and indexing
void shrinkOriginMap(std::vector<std::vector<int>>& origin_map, std::vector<int>& seams, int w, int h) {
    for (int i = 0; i < h; i++) {
        int c = seams[i];
        origin_map[i].erase(origin_map[i].begin() + c);
    }
}

// ---------- Graph Cut method ----------
// Node ID calculation based on x, y coords + width
int id(int i, int j, int W) { return i * W + j; }

// Sentinel node for row i, sitting just right of the last pixel (column W).
// Needed because a row of W pixels needs W cuttable edges, but W nodes only give W-1.
// Without it the last column can never be picked as the seam. IDs come after s (W*H) and t (W*H+1).
int sentinel(int i, int W, int H) { return W * H + 2 + i; }

// Reversion of Node ID, going back to a pixel
std::tuple<int, int> reverse_id(int u, int W) {
    // int i = u / W;   // row
    // int j = u % W;   // column
    return std::tuple<int, int>{
        u / W, u % W
    };
}

struct Edge {
    int to;        // the id of the node where it points
    long long cap; // remaining capacity
};

// possible edges:
// NW N  NE          NW x  x        *(i-1, j-1)  (i-1, j)  (i-1, j+1)
// W  C  E    ====>  W  C  E  ====> *(i,   j-1) *(i,   j) *(i,   j+1)
// SE S  SE          SW x  x        *(i+1, j-1)  (i+1, j)  (i+1, j+1)
// E is the only one that carries the cost (capacity, finite edge)
// W stops a row from being cut twice
// SW and NW are infinite and keeps the seam connected between rows

std::vector<Edge> edges;               // all edges, reverse stored right after each one
std::vector<std::vector<int>> adj;     // adj[u] = indices into edges struct for edges leaving u

// s and t are node IDs. By default they take the values W*H and W*H+1 since these two values are out
// of range for all possible node IDs in the graph. Setting them to 0 and 1 would technically be fine too
// but since that would shift up all the IDs in the image by 2, I would rather not confuse myself with the
// shifting and instead always leave them consistently at W*H and W*H+1
int s;                                 // source, a node connected by an inf-cap edge to first col's px in every row
int t;                                 // sink, a node w an inf-cap edge from every row's sentinel node (just right of the last col)

const long long INF = LLONG_MAX / 4; // bigger than any possible seam cost

double totalms;

// Add Edge Helper Function
void addEdge(int u, int v, long long cap) {
    // storing the edge and its capacity
    adj[u].push_back(edges.size());
    edges.push_back({v, cap});

    // then the reverse of the edge and 0
    adj[v].push_back(edges.size());
    edges.push_back({u, 0});
}

// Build the Edges and Adjacency maps here based on the EnergyMap
void build_edge_adj(const imgData& imd, const EnergyMap& em) {
    int W = imd.getWidth(); int H = imd.getHeight();

    // clear the globals here
    edges.clear();
    adj.assign(W * H + 2 + H, {});   // pixels + s + t + one sentinel node per row

    // 1. source edges:   s -> (i, 0)          for each row
    //                    (one infinite pipe from s to px in first col)
    for (int i = 0; i < H; i++) addEdge(s, id(i, 0, W), INF);

    // 2. sink edges via a sentinel node k just right of the last column, for each row:
    //      (i, W-1) -> k   cap = em[i][W-1]   (east edge of the last pixel, so it can be the seam too)
    //      k -> (i, W-1)   cap = INF          (west edge, same role as step 4)
    //      k -> t          cap = INF          (the sink now hangs off the sentinels)
    //      k -> (i-1, W-1) and (i+1, W-1)  cap = INF   (NW/SW edges, same role as step 5)
    for (int i = 0; i < H; i++) {
        int k = sentinel(i, W, H);
        addEdge(id(i, W - 1, W), k, em[i][W - 1]);
        addEdge(k, id(i, W - 1, W), INF);
        addEdge(k, t, INF);
        if (i > 0)     addEdge(k, id(i - 1, W - 1, W), INF);
        if (i + 1 < H) addEdge(k, id(i + 1, W - 1, W), INF);
    }

    // 3. east edges:     (i,j) -> (i,j+1)     cap = em[i][j]
    // 4. west edges:     (i,j+1) -> (i,j)     cap = INF
    for (int i = 0; i < H; i++) {
        for (int j = 0; j + 1 < W; j++) {
            addEdge(id(i, j, W),     id(i, j + 1, W), em[i][j]);  // east: cost of cutting here (finite)
            addEdge(id(i, j + 1, W), id(i, j, W),     INF);       // west: stops a row being cut twice (infinite)
        }
    }

    // 5. NW/SW edges:    (i,j) -> (i-1,j-1) and (i+1,j-1), cap = INF
    //                    (only where j >= 1 and the target row exists)
    for (int i = 0; i < H; i++) {
        for (int j = 1; j < W; j++) {
            if (i > 0)     addEdge(id(i, j, W), id(i - 1, j - 1, W), INF);  // NW
            if (i + 1 < H) addEdge(id(i, j, W), id(i + 1, j - 1, W), INF);  // SW
        }
    }
}

// Dinic: BFS once to label nodes by distance from s, then DFS pushes many paths
// along edges that go exactly one level deeper. Repeat until t is unreachable.
// Compared to Edmonds-Karp (one BFS per augmenting path), one BFS here is reused for many paths,
// which is why it is a lot faster on this graph.
//
// level[u] = BFS distance from s to u in the residual graph (-1 = not reachable)
// it[u]    = index of the next edge in adj[u] still worth trying (the "current arc").
//            Edges before it[u] are saturated or lead to dead ends, so they are never retried
//            within the same phase.
std::vector<int> level, it;

// Builds the level graph. Only residual edges (cap > 0) count.
// Returns false when t can no longer be reached = no augmenting path left = flow is maximum.
bool bfsLevel(int V) {
    level.assign(V, -1);
    std::queue<int> q;
    q.push(s); level[s] = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int ei : adj[u]) {
            int v = edges[ei].to;
            // residual capacity left and not visited yet -> v is one level deeper than u
            if (edges[ei].cap > 0 && level[v] < 0) {
                level[v] = level[u] + 1;
                q.push(v);
            }
        }
    }
    return level[t] >= 0;
}

// Pushes one augmenting path from u to t, along edges that go exactly one level deeper.
// f = the bottleneck (smallest residual capacity) seen so far along the path.
// Returns the amount of flow pushed, or 0 if u cannot reach t anymore in this level graph.
long long dfsPush(int u, long long f) {
    if (u == t) return f;   // reached the sink: f is the bottleneck of the whole path
    for (int &k = it[u]; k < (int)adj[u].size(); k++) {   // it[u] remembers dead ends
        int ei = adj[u][k], v = edges[ei].to;
        // only follow edges with capacity left that go exactly one level deeper
        if (edges[ei].cap > 0 && level[v] == level[u] + 1) {
            long long d = dfsPush(v, std::min(f, edges[ei].cap));
            if (d > 0) {
                // on the way back up, update the residual graph:
                edges[ei].cap     -= d;   // less room going forward
                edges[ei ^ 1].cap += d;   // reverse edge sits at index ^ 1 (edges are stored in pairs),
                                          // so flow can be undone later if a better route is found
                return d;
            }
            // d == 0: v is a dead end, so the loop's k++ moves past this edge and it[u] keeps it skipped
            // (on success we return without k++, so the same edge is retried in case it still has capacity)
        }
    }
    return 0;   // no edge from u leads to t
}

// Runs Dinic: each outer iteration is one "phase" (rebuild levels, then saturate the level graph).
// Stops once bfsLevel finds no path, at which point the flow is maximum = min cut value.
void maxFlow(int V) {
    while (bfsLevel(V)) {
        it.assign(V, 0);                       // reset current-arc pointers for the new phase
        while (dfsPush(s, INF) > 0) {}         // keep pushing paths until the level graph is blocked
    }
}

std::vector<int> findSeam(const imgData& imd, const EnergyMap& em) {
    int W = imd.getWidth(), H = imd.getHeight(), V = W * H + 2 + H;   // + H sentinel nodes

    std::cout << "calling maxflow\n";
    maxFlow(V);

    // BFS from s over leftover capacity = source side (S) of the min cut.
    // After max flow, every edge crossing from S to the rest is saturated; those are the cut edges.
    std::vector<bool> reach(V, false);
    std::queue<int> q;
    q.push(s); reach[s] = true;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int ei : adj[u]) {
            int v = edges[ei].to;
            if (edges[ei].cap > 0 && !reach[v]) { reach[v] = true; q.push(v); }
        }
    }

    // seam pixel in each row = last reachable column (rightmost pixel in S).
    // The cut edge in each row is (i,j)->(i,j+1) with j in S and j+1 not, so j is the pixel whose
    // removal cost was paid. The infinite west edges guarantee S is contiguous from column 0 in each row.
    std::vector<int> seam(H, 0);
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            if (reach[id(i, j, W)]) seam[i] = j;
    return seam;
}
// ---------- Graph Cut method ----------

// ./q4 ../img/BroadwayTowerSeamCarvingA.png 100 broadway.png broadway_energy.png broadway_seam.png
// ./q4 ../img/content-aware-scale-cat.png 100 cat.png cat_energy.png cat_seam.png
// main takes: image path, number of seams, output path, energy map output path, seam output path, optional "h" for horizontal seams
// ./q4 ../img/content-aware-scale-cat.png 50 cat_h.png cat_energy.png cat_seam.png h
int main(int argc, char *argv[])
{
	//std::cout << "hello world\n";
    //energy_map::test();
    //test_stb_included();
    
    // Step 1: Read image with stb_image
    // energy_map::Image image;
    // if (!energy_map::read_image(image, "../img/BroadwayTowerSeamCarvingA.png")) {
    //     exit(1);
    // }
    std::string imgpath = argv[1];
    imgData imd{};
    if (!imd.parseImg(imgpath))
    {
        std::cout << "Exiting..." <<std::endl;
        return 0;
    }

    int max = 0;
    // Step 2: Apply the energy filter to it
    EnergyMap em = buildEnergyMap(imd, max);

    imgData seamimd{};

    // energy map is output in the original orientation
    energyMapOutput(imd, max, em, argv[4], seamimd);

    //Create a origin map to store the original column indexes
    std::vector<std::vector<int>> origin_map{};
    origin_map.resize(imd.getHeight());
    
    //Create all seams to store all the seams for the output
    std::vector<std::vector<int>> allseams{};
    allseams.resize(imd.getHeight());
    for (int r = 0; r < imd.getHeight(); r++) {
        origin_map[r].resize(imd.getWidth());
        for (int c = 0; c < imd.getWidth(); c++) {
            origin_map[r][c] = c;
        }
            
    }

    // optional 5th arg "h" = remove horizontal seams. A horizontal seam in the original
    // image is a vertical seam in the transposed image, so we transpose, carve vertically,
    // then transpose back at the end.
    bool horizontal = (argc > 6 && std::string(argv[6]) == "h");
    if (horizontal)
    {
        imd.transpose();
        em = buildEnergyMap(imd, max); // old map has the wrong shape after transposing
    }

    // Step 3: Perform Graph Cut & Discard the removed seams
    // restrict actual seam count here depending on picture's dimensions, because if it goes beyond width, segfault happens
    int seamCount = std::stoi(argv[2]);
    int maxW = (seamCount >= imd.getWidth() ? imd.getWidth() - 1 : seamCount);

    for (int i=0; i < maxW; i++)
    {
        
        //Record the start time
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        std::cout << "------------------------------ITERATION "<< i +1<<"-------------------------------------\n";
        std::cout << "this is iteration " << i +1 << " of " << maxW << std::endl;
        // compute s and t
        // they can have arbitrary node IDs, but since 0 and 1 are taken, 
        // they take the first 2 out of bounds IDs (being W*H and W*H+1)
        //std::cout << "recomputing s and t's indices...\n";
        s = imd.getWidth() * imd.getHeight();
        t = s + 1;

        // build the edge and adjacency lists
        //std::cout << "building edge and adjacency lists...\n";
        build_edge_adj(imd, em);

        // max flow, bfs, seam extraction
        //std::cout << "finding a seam...\n";
        std::vector<int> seam = findSeam(imd, em);
        
        //Get the original coordinates
        std::vector<int> originalco = getOriginal(seam, imd.getWidth(), imd.getHeight(), origin_map);

        //add seams to allseam list
        addSeamsToAll(allseams, imd.getHeight(), originalco);

        //Shrink the origin_map
        shrinkOriginMap(origin_map, seam, imd.getWidth(), imd.getHeight());

        // remove seam from image
        //std::cout << "removing a seam from img...\n";
        imd.removeSeam(seam);

        // rebuild the energy map for the next iteration
        //std::cout << "rebuilding energy map for next iteration\n";
        em = buildEnergyMap(imd, max);
        //Record the end time
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        //Cast the difference between start to end 
        double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        
        std::cout << "Time Elapsed for iteration " << i+ 1<< ": " << duration<<" msec" << std::endl;
        std::cout << "------------------------------------------------------------------------------\n";

        //Add it up to total
        totalms += duration;

    }

    // Step 4: Output Image
    // where to output, target width, target height, target channels, bytes, 
    // if (test_img != nullptr) {
    //     energy_map::write_image("abc.png", image.img_width, image.img_height, image.img_channels, test_img);
    // }
    
    // free(test_img);
    // free_image(image);
    std::cout << "writing image...\n";
    if (horizontal) imd.transpose(); // back to the original orientation
    std::string imgout = argv[3];
    imd.writeImg(imgout);
    //Output the seams 
    seamOutput(seamimd, argv[5], allseams);
    std::cout << "Time Elapsed Average: ";
    //Get the average of the total
    std::cout<< totalms / maxW << " ms"; // changed to maxW here instead of the arg because number of seams
    std::cout << std::endl;
}