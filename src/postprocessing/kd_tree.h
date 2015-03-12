#ifndef __KD_TREE_H__
#define __KD_TREE_H__

#include "../utils/helper.h"
#include <queue>

struct Point
{
    vector<double> x;
    string name;
    
    Point () {
        name = "NULL";
    }
    
    Point (string _name, vector<double> _x) {
        name = _name;
        x = _x;
    }
    
    int size() const {
        return x.size();
    }
    
    double operator [](int index) const {
        return x[index];
    }
    
    double& operator [](int index) {
        return x[index];
    }
};

bool operator != (const Point &a, const Point &b)
{
    return a.name != b.name;
}

int pivot;

bool byPivot(const Point &a, const Point &b)
{
    for (int shift = 0, i = pivot; shift < a.size(); ++ shift) {
        if (a[i] + EPS < b[i]) {
            return true;
        }
        if (a[i] - EPS > b[i]) {
            return false;
        }
        ++ i;
        if (i == a.size()) {
            i = 0;
        }
    }
    return a.name < b.name;
}

class KDTree
{
    vector<Point> points;
    vector<Point> mini, maxi;
    int d;
    
    void build(int num, int l, int r, int pivot) {
        if (pivot == d) {
            pivot = 0;
        }
        int mid = l + r >> 1;
        ::pivot = pivot;
        nth_element(points.begin() + l, points.begin() + mid, points.begin() + r, byPivot);
        mini[num] = maxi[num] = points[mid];
        
        if (l < mid) {
            build(num * 2, l, mid, pivot + 1);
            for (int i = 0; i < d; ++ i) {
                mini[num][i] = min(mini[num][i], mini[num * 2][i]);
                maxi[num][i] = max(maxi[num][i], maxi[num * 2][i]);
            }
        }
        if (mid + 1 < r) {
            build(num * 2 + 1, mid + 1, r, pivot + 1);
            for (int i = 0; i < d; ++ i) {
                mini[num][i] = min(mini[num][i], mini[num * 2 + 1][i]);
                maxi[num][i] = max(maxi[num][i], maxi[num * 2 + 1][i]);
            }
        }
    }
    
    double estimate(const Point &mini, const Point &maxi, const Point &target) const {
        double s = 0;
        for (int i = 0; i < d; ++ i) {
            if (target[i] < mini[i]) {
                s += sqr(target[i] - mini[i]);
            } else if (target[i] > maxi[i]) {
                s += sqr(target[i] - maxi[i]);
            }
        }
        return s;
    }
    
    void update(const Point &a, const Point &b, int k, priority_queue<pair<double, string>> &heap) const {
        double s = 0;
        for (int i = 0; i < d; ++ i) {
            s += sqr(a[i] - b[i]);
        }
        if (heap.size() < k || s < heap.top().first) {
            heap.push(make_pair(s, a.name));
            if (heap.size() > k) {
                heap.pop();
            }
        }
    }
    
    void query(int num, int l, int r, int pivot, const Point &target, int k, priority_queue<pair<double, string>> &heap) const {
        if (l >= r || heap.size() == k && estimate(mini[num], maxi[num], target) >= heap.top().first) {
            return;
        }

        if (pivot == d) {
            pivot = 0;
        }
        int mid = l + r >> 1;
        ::pivot = pivot;
        
        if (target != points[mid]) {
            update(points[mid], target, k, heap);
        }

        if (byPivot(target, points[mid])) {
            query(num * 2, l, mid, pivot + 1, target, k, heap);
            query(num * 2 + 1, mid + 1, r, pivot + 1, target, k, heap);
        } else {
            query(num * 2 + 1, mid + 1, r, pivot + 1, target, k, heap);
            query(num * 2, l, mid, pivot + 1, target, k, heap);
        }
    }
public:
    KDTree(const vector<Point> &points) : points(points) {
        maxi.resize(points.size() * 4);
        mini.resize(points.size() * 4);
        if (points.size() > 0) {
            d = points[0].size();
            build(1, 0, points.size(), 0);
            cerr << "KD Tree built, dimension = " << d << endl;
            cerr << "# points = " << points.size() << endl;
        } else {
            cerr << "[Warning] 0 points in KD Tree" << endl;
        }
    }
    
    vector<string> query(const Point &target, int k) const {
        priority_queue<pair<double, string>> heap;
        query(1, 0, points.size(), 0, target, k, heap);
        vector<string> ret;
        while (heap.size() > 0) {
            ret.push_back(heap.top().second);
            heap.pop();
        }
        return ret;
    }
};

#endif
