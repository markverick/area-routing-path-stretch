#include<bits/stdc++.h>
using namespace std;

using namespace std;

int adj[1600][1600];
// int dx[4] = {0, 1, 0, -1};
// int dy[4] = {1, 0, -1, 0};
int gridHeight, gridWidth, areaHeight, areaWidth;
int srcX, srcY, dstX, dstY;
int dstAX, dstAY;
const int INF = 1000000000;
const int E6 = 1000000;
class Stat {
private:
	double m_count;     // count can overflow
	double m_min;
	double m_max;
	double m_avg;
	double m_avg_hop;
	double m_min_hop;
	double m_max_hop;

public:
	Stat()
		: m_count(-1), m_min(-1), m_max(-1), m_avg(-1),
		  m_avg_hop(-1), m_min_hop(-1), m_max_hop(-1) {}

	Stat(double count, double min_value, double max_value, double avg_value,
	     double avg_hop, double min_hop, double max_hop)
		: m_count(count), m_min(min_value), m_max(max_value), m_avg(avg_value),
		  m_avg_hop(avg_hop), m_min_hop(min_hop), m_max_hop(max_hop) {}

	// Getters
	double GetCount()   const { return m_count; }
	double GetMin()     const { return m_min; }
	double GetMax()     const { return m_max; }
	double GetAvg()     const { return m_avg; }
	double GetAvgHop()  const { return m_avg_hop; }
	double GetMinHop()  const { return m_min_hop; }
	double GetMaxHop()  const { return m_max_hop; }

	void Clear() {
		m_count = -1;
		m_min = m_max = m_avg = -1;
		m_avg_hop = m_min_hop = m_max_hop = -1;
	}

	void Print() const {
		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Statistics Summary:\n";
		std::cout << "  Count:     " << m_count << "\n";
		std::cout << "  Min:       " << m_min << "\n";
		std::cout << "  Max:       " << m_max << "\n";
		std::cout << "  Avg:       " << m_avg << "\n";
		std::cout << "  Avg Hop:   " << m_avg_hop << "\n";
		std::cout << "  Min Hop:   " << m_min_hop << "\n";
		std::cout << "  Max Hop:   " << m_max_hop << "\n";
	}

	// Shift values by (distance, hop)
	Stat operator+(std::pair<double, double> pair) const {
		double dist = pair.first;
		double hop = pair.second;
		return Stat(m_count,
		            m_min + dist, m_max + dist, m_avg + dist,
		            m_avg_hop + hop, m_min_hop + hop, m_max_hop + hop);
	}

	bool operator<(const Stat& other) const {
		return m_avg < other.m_avg;
	}

	bool operator==(const Stat& other) const {
		return m_count == other.m_count &&
		       m_min == other.m_min &&
		       m_max == other.m_max &&
		       m_avg == other.m_avg &&
		       m_avg_hop == other.m_avg_hop &&
		       m_min_hop == other.m_min_hop &&
		       m_max_hop == other.m_max_hop;
	}
};

Stat dp[1600][1600];

Stat aggregateStats(const std::vector<Stat>& stats) {
	double mn = INF;
	double mx = 0;
	double hopMn = INF;
	double hopMx = 0;
	double count = 0;
	double sum = 0;
	double hopSum = 0;

	for (const auto& stat : stats) {
		double c = stat.GetCount();
		if (c <= 0) continue;

		count += c;
		mn = std::min(mn, stat.GetMin());
		mx = std::max(mx, stat.GetMax());
		hopMn = std::min(hopMn, stat.GetMinHop());
		hopMx = std::max(hopMx, stat.GetMaxHop());
		sum += stat.GetAvg() * c;
		hopSum += stat.GetAvgHop() * c;
	}

	if (count == 0)
		return Stat(0, 0, 0, 0, 0, 0, 0); // or return a sentinel Stat

	double avg = sum / count;
	double avg_hop = hopSum / count;
	return Stat(count, mn, mx, avg, avg_hop, hopMn, hopMx);
}

int xBound(int x) {
	while (x < 0) x += gridHeight;
	while (x >= gridHeight) x -= gridHeight;
	return x;
}

int yBound(int y) {
	while (y < 0) y += gridWidth;
	while (y >= gridWidth) y -= gridWidth;
	return y;
}

int toNodeId(int x, int y) {
	x = xBound(x);
	y = yBound(y);
	return x * gridWidth + y;
}

double getDistance(int curX, int curY, int dstX, int dstY) {
	if (curX != dstX && curY != dstY) {
		cout << "Only one-axis-distance is supported" << std::endl;
		return 0;
	}
	int a,b,i;
	double sm = 0;
	if (curX == dstX && curY == dstY) {
		return 0;
	}
	// Vertical Distance
	if (curY == dstY) {
		a = min(curX, dstX);
		b = max(curX, dstX);
		for (i = a; i < b; i++) {
			int cur = i * gridWidth + curY;
			int dst = (i + 1) * gridWidth + dstY;
			sm += adj[cur][dst];
		}
	}
	if (curX == dstX) {
		a = min(curY, dstY);
		b = max(curY, dstY);
		for (i = a; i < b; i++) {
			int cur = curX * gridWidth + i;
			int dst = dstX * gridWidth + i + 1;
			sm += adj[cur][dst];
		}
	}
	return sm;
}

Stat getEqualPathStat (int curX, int curY) {
	// Base Case
	if (curX == dstX && curY == dstY) {
		return Stat (1, 0, 0, 0, 0, 0, 0);
	}

	// Memorized
	if (dp[curX][curY].GetMin() != -1) {
		return dp[curX][curY];
	}
	// cout << "curX: " << curX << ", curY: " << curY << endl;

	// Get current area
	int curAX = curX / areaHeight;
	int curAY = curY / areaWidth; 

	// Direction (s) to go
	vector<pair<int, double>> dx, dy;

	// In the same area
	if (curAX == dstAX && curAY == dstAY) {
		int leftDist, rightDist, upDist, downDist;
		// Vertical
		if (curX > dstX) {
			upDist = curX - dstX;
			downDist = areaHeight == gridHeight? dstX + gridHeight - curX : INF; // cannot go if not stripe
		} else {
			upDist = areaHeight == gridHeight? curX + gridHeight - dstX : INF; // cannot go if not stripe
			downDist = dstX - curX;
		}
		// Horizontal
		if (curY > dstY) {
			leftDist = curY - dstY;
			rightDist = areaWidth == gridWidth? dstY + gridWidth - curY : INF; // cannot go if not stripe
		} else {
			leftDist = areaWidth == gridWidth? curY + gridWidth - dstY : INF; // cannot go if not stripe
			rightDist = dstY - curY;
		}

		// Direction(s) to go
		// Vertical
		if (downDist > 0 && downDist <= upDist) {
			dx.emplace_back(1, adj[toNodeId(curX, curY)][toNodeId(curX + 1, curY)]);
		}
		if (upDist > 0 && upDist <= downDist) {
			dx.emplace_back(-1, adj[toNodeId(curX, curY)][toNodeId(curX - 1, curY)]);
		}
		// Horizontal
		if (rightDist > 0 && rightDist <= leftDist) {
			dy.emplace_back(1, adj[toNodeId(curX, curY)][toNodeId(curX, curY + 1)]);
		}
		if (leftDist > 0 && leftDist <= rightDist) {
			dy.emplace_back(-1, adj[toNodeId(curX, curY)][toNodeId(curX, curY - 1)]);
		}
	} else {
		// Cross Area
		int leftDist, rightDist, upDist, downDist;
		int leftHop, rightHop, upHop, downHop;
		upHop = curX % areaHeight;
		if (curAX < gridHeight / areaHeight) {
			downHop = areaHeight - upHop - 1;
		} else {
			// Remainder area
			downHop = gridHeight % areaHeight - upHop - 1;
		}
		leftHop = curY % areaWidth;
		if (curAY < gridWidth / areaWidth) {
			rightHop = areaWidth - leftHop - 1;
		} else {
			rightHop = gridHeight % areaHeight - leftHop - 1;
		}
		// cout << upHop << "," << downHop << "," << leftHop << "," << rightHop << endl;
		// Vertical
		if (curAX > dstAX) {
			upDist = (curAX - dstAX) * areaHeight + upHop;
			downDist = (dstAX + gridHeight - curAX) * areaHeight + downHop;
		} else if (curAX < dstAX) {
			upDist = (curAX + gridHeight - dstAX) * areaHeight + upHop;
			downDist = (dstAX - curAX) * areaHeight + downHop;
		} else {
			upDist = 0;
			downDist = 0;
		}
		// Horizontal
		if (curAY > dstAY) {
			leftDist = (curAY - dstAY) * areaWidth + leftHop;
			rightDist = (dstAY + gridWidth - curAY) * areaWidth + rightHop;
		} else if (curAY < dstAY) {
			leftDist = (curAY + gridWidth - dstAY) * areaWidth + leftHop;
			rightDist = (dstAY - curAY) * areaWidth + rightHop;
		} else {
			leftDist = 0;
			rightDist = 0;
		}
		// Direction(s) to go
		// Vertical
		double dist;
		int hop;
		if (downDist > 0 && downDist <= upDist) {
			hop = downHop + 1;
			dist = getDistance(curX, curY, xBound(curX + hop), curY);
			dx.emplace_back(hop, dist); // Beeline to the next area
		}
		if (upDist > 0 && upDist <= downDist) {
			hop = -upHop - 1;
			dist = getDistance(curX, curY, xBound(curX + hop), curY);
			dx.emplace_back(hop, dist); // Beeline to the next area
		}
		// Horizontal
		if (rightDist > 0 && rightDist <= leftDist) {
			hop = rightHop + 1;
			dist = getDistance(curX, curY, curX, yBound(curY + hop));
			dy.emplace_back(hop, dist); // Beeline to the next area
		}
		if (leftDist > 0 && leftDist <= rightDist) {
			hop = -leftHop - 1;
			dist = getDistance(curX, curY, curX, yBound(curY + hop));
			dy.emplace_back(hop, dist); // Beeline to the next area
		}
	}
	int i, j;
	int nextX, nextY;
	double mn = INF;
	double mx = 0;
	double avg = 0;
	vector<Stat> stats;
	Stat tmp;
	for (auto& [deltaX, dist] : dx) {
		nextX = xBound(curX + deltaX);
		nextY = curY;
		if (dp[nextX][nextY].GetMin() != -1) {
			stats.emplace_back(dp[nextX][nextY] + make_pair(dist, abs(deltaX)));
		} else {
			tmp = getEqualPathStat(nextX, nextY) + make_pair(dist, abs(deltaX));
			stats.emplace_back(tmp);
			dp[nextX][nextY] = tmp;
		}
	}
	for (auto& [deltaY, dist] : dy) {
		nextX = curX;
		nextY = yBound(curY + deltaY);
		if (dp[nextX][nextY].GetMin() != -1) {
			stats.emplace_back(dp[nextX][nextY] + make_pair(dist, abs(deltaY)));
		} else {
			tmp = getEqualPathStat(nextX, nextY) + make_pair(dist, abs(deltaY));
			stats.emplace_back(tmp);
			dp[nextX][nextY] = tmp;
		}
	}
	return dp[curX][curY] = aggregateStats(stats);
}

int getMinHopCount(int sx, int sy, int dx, int dy) {
	// Compute horizontal (x-axis) distance with wrapping
    int dx1 = abs(dx - sx);
    int dx2 = gridHeight - dx1;
    int minDx = std::min(dx1, dx2);

    // Compute vertical (y-axis) distance with wrapping
    int dy1 = abs(dy - sy);
    int dy2 = gridWidth - dy1;
    int minDy = std::min(dy1, dy2);

    // Total hops is sum of min distance in each dimension
    return minDx + minDy;
}

Stat getStatOnePair (int sx, int sy, int dx, int dy) {
	srcX = sx;
	srcY = sy;
	dstX = dx;
	dstY = dy;
	// Area
	dstAX = dstX / areaHeight;
	dstAY = dstY / areaWidth;
	int i,j;
	for (i = 0; i < gridHeight; i++) {
		for (j = 0; j < gridWidth; j++) {
			dp[i][j].Clear();
		}
	}
	// cout << srcX << "," << srcY << " -> " << dstX << "," << dstY << endl;
	// cout << srcX / areaHeight << "," << srcY / areaWidth << " -> " << dstX / areaHeight << "," << dstY / areaWidth<< endl;
	auto stat = getEqualPathStat (srcX, srcY);
	// stat.Print();
	return stat;
}
int getMinHopCount(int src, int dst) {
	return getMinHopCount (src / gridWidth, src % gridWidth, dst / gridWidth, dst % gridWidth);
}

Stat getStatOnePair (int src, int dst) {
	return getStatOnePair (src / gridWidth, src % gridWidth, dst / gridWidth, dst % gridWidth);
}

void computeStatsRandomPairs(int n, string outputFileName) {
	srand(2025);
	vector<tuple<int, int, int, Stat> > stats;
	for (int i = 0; i < n; i++) {
		int node1 = rand() % (gridHeight * gridWidth);
		int node2 = rand() % (gridHeight * gridWidth - 1);
		if (node2 >= node1) node2++;
		// cout << "Nodes [" << node1 << "," << node2 << "]" << endl;
		auto hop = getMinHopCount(node1, node2);
		auto stat = getStatOnePair(node1, node2);
		stats.emplace_back(hop, node1, node2, stat);
	}
	sort(stats.begin(), stats.end());
	std::ofstream outfile(outputFileName);
	if (!outfile) {
        std::cerr << "Failed to open file.\n";
        return;
    }
	outfile << "hop_count,src,dst,min,avg,max,path_count,avg_hop,min_hop,max_hop" << std::endl;

	for (auto& [hop, node1, node2, stat] : stats) {
		outfile << hop << ","
				<< node1 << ","
				<< node2 << ","
				<< stat.GetMin() / E6 << ","
				<< stat.GetAvg() / E6 << ","
				<< stat.GetMax() / E6 << ","
				<< stat.GetCount() << ","
				<< stat.GetAvgHop() << ","
				<< stat.GetMinHop() << ","
				<< stat.GetMaxHop() << std::endl;

		// Optional debug print
		// std::cout << "  [" << hop << "] "
		// 		<< stat.GetMin() / E6 << " <- "
		// 		<< stat.GetAvg() / E6 << " -> "
		// 		<< stat.GetMax() / E6
		// 		<< " (hops: " << stat.GetMinHop()
		// 		<< " <- " << stat.GetAvgHop()
		// 		<< " -> " << stat.GetMaxHop() << ")\n";
	}
}

int main () {
	freopen("topo.txt", "r", stdin);
	int n,i,j,k,ia,ib,ic;
	scanf("%d", &n);

	gridHeight = 72;
	gridWidth = 22;

	vector<int> heightCandidates = {72, 36, 18, 8, 4, 2};
	vector<int> widthCandidates = {22, 11, 4};
	vector<int> areaHeights;
	vector<int> areaWidths;
	for (auto h : heightCandidates) {
		for (auto w : widthCandidates) {
			areaHeights.emplace_back(h);
			areaWidths.emplace_back(w);
		}
	}

	for (i=0;i<areaHeights.size();i++) {
		areaHeight = areaHeights[i];
		areaWidth = areaWidths[i];
		for (j=0;j<n;j++) {
			scanf("%d %d %d",&ia,&ib,&ic);
			adj[ia][ib]=ic;
			adj[ib][ia]=ic;
		}

		string outputFile = "results/" + to_string(areaHeight) + "x" + to_string(areaWidth) + ".csv";
		computeStatsRandomPairs(100, outputFile);
	}
	return 0;
}

