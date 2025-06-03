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
class Stat {
private:
	double m_min;
	double m_max;
	double m_avg;
	int m_count;

public:
	Stat() : m_min(-1), m_max(-1), m_avg(-1), m_count(-1) {}

	Stat(double min_value, double max_value, double avg_value, int count)
		: m_min(min_value), m_max(max_value), m_avg(avg_value), m_count(count) {}

	double GetMin() const { return m_min; }
	double GetMax() const { return m_max; }
	double GetAvg() const { return m_avg; }
	int GetCount() const { return m_count; }

	void Clear() {
		m_min = m_max = m_avg = -1;
		m_count = -1;
	}

	void Print() const {
		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Statistics Summary:\n";
		std::cout << "  Min:   " << m_min   << "\n";
		std::cout << "  Max:   " << m_max   << "\n";
		std::cout << "  Avg:   " << m_avg   << "\n";
		std::cout << "  Count: " << m_count << "\n";
	}

	Stat operator+(double value) const {
	return Stat(m_min + value, m_max + value, m_avg + value, m_count);
}
};

Stat dp[1600][1600];

Stat aggregateStats (std::vector<Stat> stats) {
	double mn = INF;
	double mx = 0;
	int count = 0;
	double sum = 0;
	for (auto stat : stats) {
		count += stat.GetCount();
		mn = min(mn, stat.GetMin());
		mx = max(mx, stat.GetMax());
		sum += stat.GetAvg() * stat.GetCount();
	}
	return Stat(mn, mx, sum / count, count);
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
		cout << "Base case" << std::endl;
		return Stat (0, 0, 0, 1);
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
		// Vertical
		if (curAX > dstAX) {
			upDist = curAX - dstAX;
			downDist = dstAX + gridHeight - curAX;
		} else {
			upDist = curAX + gridHeight - dstAX;
			downDist = dstAX - curAX;
		}
		// Horizontal
		if (curAY > dstAY) {
			leftDist = curAY - dstAY;
			rightDist = dstAY + gridWidth - curAY;
		} else {
			leftDist = curAY + gridWidth - dstAY;
			rightDist = dstAY - curAY;
		}
		// Direction(s) to go
		// Vertical
		double dist;
		int hop;
		if (downDist > 0 && downDist <= upDist) {
			hop = areaHeight - curX % areaHeight;
			dist = getDistance(curX, curY, xBound(curX + hop), curY);
			dx.emplace_back(hop, dist); // Beeline to the next area
		}
		if (upDist > 0 && upDist <= downDist) {
			hop = -1 * (curX % areaHeight + 1);
			dist = getDistance(curX, curY, xBound(curX + hop), curY);
			dx.emplace_back(hop, dist); // Beeline to the next area
		}
		// Horizontal
		if (rightDist > 0 && rightDist <= leftDist) {
			hop = areaWidth - curY % areaWidth;
			dist = getDistance(curX, curY, curX, yBound(curY + hop));
			dy.emplace_back(hop, dist); // Beeline to the next area
		}
		if (leftDist > 0 && leftDist <= rightDist) {
			hop = -1 * (curY % areaWidth + 1);
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
			stats.emplace_back(dp[nextX][nextY] + dist);
		} else {
			tmp = getEqualPathStat(nextX, nextY) + dist;
			stats.emplace_back(tmp);
			dp[nextX][nextY] = tmp;
		}
	}
	for (auto& [deltaY, dist] : dy) {
		nextX = curX;
		nextY = yBound(curY + deltaY);
		if (dp[nextX][nextY].GetMin() != -1) {
			stats.emplace_back(dp[nextX][nextY] + dist);
		} else {
			tmp = getEqualPathStat(nextX, nextY) + dist;
			stats.emplace_back(tmp);
			dp[nextX][nextY] = tmp;
		}
	}
	return dp[curX][curY] = aggregateStats(stats);
}

int main () {
	freopen("topo.txt", "r", stdin);
	int n,i,j,k,ia,ib,ic;
	scanf("%d", &n);

	gridHeight = 72;
	gridWidth = 22;
	areaHeight = 8;
	areaWidth = 11;

	for (i=0;i<n;i++) {
		scanf("%d %d %d",&ia,&ib,&ic);
		adj[ia][ib]=ic;
		adj[ib][ia]=ic;
	}

	srcX = 0;
	srcY = 0;
	dstX = 2;
	dstY = 2;

	// Area
	dstAX = dstX / areaHeight;
	dstAY = dstY / areaWidth;

	for (i = 0; i < gridHeight; i++) {
		for (j = 0; j < gridWidth; j++) {
			dp[i][j].Clear();
		}
	}
	auto stat = getEqualPathStat (srcX, srcY);
	stat.Print();
	return 0;
}

