#ifndef BEZIER_HPP
#define BEZIER_HPP

#include "Vector2f.h"
#include "object3d.hpp"
#include <algorithm>
#include <cmath>
#include <tuple>
#include <cassert>

#ifndef EPS
#define EPS 1e-3
#endif

#define sqr(a) ((a) * (a))
#define fmax(a, b) ((a) > (b) ? (a) : (b))
#define fmin(a, b) ((a) < (b) ? (a) : (b))

class BezierCurve2D
{
public:
    double *dx, *dy, max, height, max2, r, num;
    int n;
    struct D
    {
        double t0, t1, width, y0, y1, width2;
    } data[20];

    // x(t) = \sum_{i=0}^n dx_i * t^i
    // y(t) = \sum_{i=0}^n dy_i * t^i
    /*
		初始化一条二维平面上的Bezier曲线
		v:		控制点坐标
		n:		控制点数量
		num:	Bezier曲线次数
		r:		微小扰动
	*/
    BezierCurve2D(Vector2f *v, int n, int num, double r = 0.365) : num(num), n(n), r(r)
    {
        double *px = new double[n];
        double *py = new double[n];
        for (int i = 0; i < n; ++i)
        {
            px[i] = v[i].x();
            py[i] = v[i].y();
        }
        dx = new double[n];
        dy = new double[n];
        assert(std::fabs(py[0]) <= EPS);
        --n;
        // preproces
        for (int i = 0; i <= n; ++i)
        {
            dx[i] = px[0];
            dy[i] = py[0];
            for (int j = 0; j <= n - i; ++j)
            {
                px[j] = px[j + 1] - px[j];
                py[j] = py[j + 1] - py[j];
            }
        }
        double n_down = 1, fac = 1, nxt = n;
        for (int i = 0; i <= n; ++i, --nxt)
        {
            fac = fac * (i == 0 ? 1 : i);
            dx[i] = dx[i] * n_down / fac;
            dy[i] = dy[i] * n_down / fac;
            n_down *= nxt;
        }
        max = 0;
        double interval = 1. / (num - 1), c = 0;
        for (int cnt = 0; cnt <= num; c += interval, ++cnt)
        {
            data[cnt].width = 0;
            data[cnt].t0 = fmax(0., c - r);
            data[cnt].t1 = fmin(1., c + r);
            data[cnt].y0 = getValue(data[cnt].t0).y();
            data[cnt].y1 = getValue(data[cnt].t1).y();
            for (double t = data[cnt].t0; t <= data[cnt].t1; t += 0.00001)
            {
                Vector2f pos = getValue(t);
                if (data[cnt].width < pos.x())
                    data[cnt].width = pos.x();
            }
            if (max < data[cnt].width)
                max = data[cnt].width;
            data[cnt].width += EPS;
            data[cnt].width2 = sqr(data[cnt].width);
        }
        max += EPS;
        max2 = max * max;
        height = getValue(1).y();
        delete[] px;
        delete[] py;
    }
    ~BezierCurve2D()
    {
        delete[] dx;
        delete[] dy;
    }

    /*
		获得Bezier曲线在t的值
	*/
    Vector2f getValue(double t)
    {
        double ans_x = 0, ans_y = 0, t_pow = 1;
        for (int i = 0; i <= n; ++i)
        {
            ans_x += dx[i] * t_pow;
            ans_y += dy[i] * t_pow;
            t_pow *= t;
        }
        return Vector2f(ans_x, ans_y);
    }

    /*
		获得Bezier曲线在t的一阶导数
	*/
    Vector2f getTangent(double t)
    {
        double ans_x = 0, ans_y = 0, t_pow = 1;
        for (int i = 1; i <= n; ++i)
        {
            ans_x += dx[i] * i * t_pow;
            ans_y += dy[i] * i * t_pow;
            t_pow *= t;
        }
        return Vector2f(ans_x, ans_y);
    }

    /*
		获得Bezier曲线在t的二阶导数
	*/
    Vector2f getTangent2(double t)
    {
        double ans_x = 0, ans_y = 0, t_pow = 1;
        for (int i = 2; i <= n; ++i)
        {
            ans_x += dx[i] * i * (i - 1) * t_pow;
            ans_y += dy[i] * i * (i - 1) * t_pow;
            t_pow *= t;
        }
        return Vector2f(ans_x, ans_y);
    }
};

const double INF = 1e10;

#define sqr(a) ((a) * (a))
#define fmax(a, b) ((a) > (b) ? (a) : (b))
#define fmin(a, b) ((a) < (b) ? (a) : (b))

class BezierSurface : public Object3D
{
private:
    BezierCurve2D *curve;
    Vector3f pivot; // The bottom centriod

    /*
        解方程y(t) = yc，返回t
    */
    double solve_t(double yc)
    {
        // assert(0 <= yc && yc <= curve->height);
        double t = 0.5, ft, dft;
        for (int i = 10; i--;)
        {
            if (t < 0)
                t = 0;
            else if (t > 1)
                t = 1;
            ft = curve->getValue(t).y() - yc, dft = curve->getTangent(t).y();
            if (std::fabs(ft) < EPS)
                return t;
            t -= ft / dft;
        }
        return -1;
    }

    /*
        给定一个在曲面上的点p，返回它在曲线上的参数 (theta, t)
    */
    virtual Vector2f change_for_bezier(Vector3f inter_p)
    {
        double t = solve_t(inter_p.y() - pivot.y());
        double theta = atan2(inter_p.z() - pivot.z(), inter_p.x() - pivot.x()); // between -pi ~ pi
        if (theta < 0)
            theta += 2 * M_PI;
        return Vector2f(theta, t);
    }

    /*
        返回射线ray 和球(o, r)的交点的t值 即 ray.pointAtParameter(t) 在球上
        如果没有交点 返回-1 
    */
    double get_sphere_intersect(Ray ray, Vector3f o, double r)
    {
        Vector3f ro = o - ray.getOrigin();
        double b = Vector3f::dot(ray.getNormalizedDirection(), ro);
        double d = sqr(b) - Vector3f::dot(ro, ro) + sqr(r);
        if (d < 0)
            return -1;
        else
            d = sqrt(d);
        double t = b - d > EPS ? b - d : b + d > EPS ? b + d : -1;
        if (t < 0)
            return -1;
        return t / ray.getDirectionLength();
    }

    bool check(double low, double upp, double init, Ray ray, double a, double b, double c, double &final_dis)
    {
        double t = newton(init, a, b, c, low, upp);
        if (t <= 0 || t >= 1)
            return false;
        Vector2f loc = curve->getValue(t);
        double x = loc.x(), y = loc.y();
        double ft = x - sqrt(a * sqr(y - b) + c);
        double dis = (pivot.y() + y - ray.getOrigin().y()) / ray.getDirection().y();
        Vector3f inter_p = ray.pointAtParameter(dis);
        if (dis < final_dis)
        {
            final_dis = dis;
            return true;
        }
        return false;
    }

    /*
		牛顿迭代法求零点
	*/
    double newton(double t, double a, double b, double c, double low = EPS, double upp = 1 - EPS)
    {
        // solve sqrt(a(y(t)+pivot.y-b)^2+c)=x(t)
        // f(t) = x(t) - sqrt(a(y(t)+pivot.y-b)^2+c)
        // f'(t) = x'(t) - a(y(t)+pivot.y-b)*y'(t) / sqrt(...)
        // if t is not in [0, 1] then assume f(t) is a linear function
        double ft, dft, x, y, dx, dy, sq;
        Vector2f loc, dir;
        for (int i = 10; i--;)
        {
            if (t < 0)
                t = low;
            if (t > 1)
                t = upp;
            loc = curve->getValue(t), dir = curve->getTangent(t);
            x = loc.x(), dx = dir.x();
            y = loc.y(), dy = dir.y();
            sq = sqrt(a * sqr(y - b) + c);
            ft = x - sq;
            dft = dx - a * (y - b) * dy / sq;
            if (std::fabs(ft) < EPS)
                return t;
            t -= ft / dft * 0.98; // 如果这里不乘0.9，则数值不稳定容易震荡
        }
        return -1;
    }

    /*
		返回射线r是射向外面还是射向旋转轴
		return 1	外侧
		return 0	旋转轴
	*/
    int getSide(Ray r)
    {
        Vector3f o = r.getOrigin();
        Vector3f oo = Vector3f(pivot.x(), o.y(), pivot.z());
        Vector3f radius = (o - oo).normalized();
        Vector3f d = r.getDirection();
        Vector3f dd = Vector3f(d.x(), 0, d.z()).normalized();
        if (Vector3f::dot(radius, dd) > EPS)
            return 1;
        return 0;
    }

    /*
        给定一个在曲面上的点p，求该处的法线方向
		side=1表示外侧，side=0表示内侧
    */
    virtual Vector3f norm(Vector3f p, int side = 1)
    {
        Vector2f tmp = change_for_bezier(p);
        Vector2f dir = curve->getTangent(tmp.y());
        Vector3f d_surface = Vector3f(cos(tmp.x()), dir.y() / dir.x(), sin(tmp.x()));
        Vector3f d_circ = Vector3f(-sin(tmp.x()), 0, cos(tmp.x()));
        Vector3f ret = Vector3f::cross(d_circ, d_surface).normalized();
        if (side != getSide(Ray(p, ret)))
            ret = -ret;
        return ret;
    }

public:
    // the curve will rotate line (x=pivot.x and z=pivot.z) as pivot
    BezierSurface(BezierCurve2D *pCurve, Material *material) : curve(pCurve), Object3D(material)
    {
        pivot = Vector3f(0, 0, 0);
    }

    ~BezierSurface() override
    {
        delete curve;
    }

    bool intersect(const Ray &ray, Hit &h, double tmin) override
    {
        double final_dis = INF;
        // check for |dy|<EPS
        if (std::fabs(ray.getDirection().y()) < EPS)
        {
            // return false;
            double dis_to_axis = (Vector3f(pivot.x(), ray.getOrigin().y(), pivot.z()) - ray.getOrigin()).length();
            double hit = ray.pointAtParameter(dis_to_axis).y();
            if (hit < pivot.y() + EPS || hit > pivot.y() + curve->height - EPS)
                return false;
            // solve function pivot.y+y(t)=ray.o.y to get x(t)
            double t = solve_t(hit - pivot.y());
            if (t < 0 || t > 1)
                return false;
            Vector2f loc = curve->getValue(t);
            double ft = pivot.y() + loc.y() - hit;
            if (std::fabs(ft) > EPS)
                return false;
            // assume sphere (pivot.x, pivot.y + loc.y, pivot.z) - loc.x
            final_dis = get_sphere_intersect(ray, Vector3f(pivot.x(), pivot.y() + loc.y(), pivot.z()), loc.x());
            if (final_dis < 0)
                return false;
            Vector3f inter_p = ray.pointAtParameter(final_dis); // 射线和Bezier曲面的交点
            if (std::fabs((inter_p - Vector3f(pivot.x(), inter_p.y(), pivot.z())).squaredLength() - sqr(loc.x())) > 1e-1)
                return false;

            // second iteration, more accuracy
            hit = inter_p.y();
            if (hit < pivot.y() + EPS || hit > pivot.y() + curve->height - EPS)
                return false;
            // solve function pivot.y+y(t)=ray.o.y to get x(t)
            t = solve_t(hit - pivot.y());
            if (t < 0 || t > 1)
                return false;
            loc = curve->getValue(t);
            ft = pivot.y() + loc.y() - hit;
            if (std::fabs(ft) > EPS)
                return false;
            // assume sphere (pivot.x, pivot.y + loc.y, pivot.z) - loc.x
            final_dis = get_sphere_intersect(ray, Vector3f(pivot.x(), pivot.y() + loc.y(), pivot.z()), loc.x());
            if (final_dis < 0)
                return false;
            inter_p = ray.pointAtParameter(final_dis); // 射线和Bezier曲面的交点
            if (std::fabs((inter_p - Vector3f(pivot.x(), inter_p.y(), pivot.z())).squaredLength() - sqr(loc.x())) > 1e-1)
                return false;

            if (final_dis >= tmin)
                if (final_dis < h.getT())
                {
                    Vector2f tmp = change_for_bezier(inter_p);
                    h.set(final_dis, material, norm(inter_p));
                    h.hasTex = true;
                    h.texCoord = Vector2f(1 - tmp.x() / 2 / M_PI, tmp.y());
                    return true;
                }
        }

        // normal case
        // calc ay^2+by+c
        double a = 0, b = 0, c = 0, t1, t2;
        // (xo-x'+xd/yd*(y-yo))^2 -> (t1+t2*y)^2
        t1 = ray.getOrigin().x() - pivot.x() - ray.getDirection().x() / ray.getDirection().y() * ray.getOrigin().y();
        t2 = ray.getDirection().x() / ray.getDirection().y();
        a += t2 * t2;
        b += 2 * t1 * t2;
        c += t1 * t1;
        // (zo-z'+zd/yd*(y-yo))^2 -> (t1+t2*y)^2
        t1 = ray.getOrigin().z() - pivot.z() - ray.getDirection().z() / ray.getDirection().y() * ray.getOrigin().y();
        t2 = ray.getDirection().z() / ray.getDirection().y();
        a += sqr(t2);
        b += 2 * t1 * t2;
        c += sqr(t1);
        // ay^2+by+c -> a'(y-b')^2+c'
        c = c - b * b / 4 / a;
        b = -b / 2 / a - pivot.y();
        if (0 <= b && b <= curve->height && c > curve->max2 || (b < 0 || b > curve->height) && std::min(sqr(b), sqr(curve->height - b)) * a + c > curve->max2) // no intersect
            return false;

        for (int ind = 0; ind <= curve->num; ++ind)
        {
            double t0 = curve->data[ind].t0, t1 = curve->data[ind].t1;
            check(t0, t1, (t0 + t1 + t0) / 3, ray, a, b, c, final_dis);
            check(t0, t1, (t1 + t0 + t1) / 3, ray, a, b, c, final_dis);
            check(t0, t1, (t1 + t0) / 2, ray, a, b, c, final_dis);
        }

        if (final_dis < INF / 2 && final_dis >= tmin && final_dis < h.getT())
        {
            Vector2f tmp = change_for_bezier(ray.pointAtParameter(final_dis));
            if (tmp.x() >= -1e5 && tmp.x() <= 1e5 && tmp.y() >= -1e5 && tmp.y() <= 1e5)
            {
                h.set(final_dis, material, norm(ray.pointAtParameter(final_dis)));
                h.hasTex = true;
                h.texCoord = Vector2f(1 - tmp.x() / 2 / M_PI, tmp.y());
                return true;
            }
        }
        return false;
    }
};

#endif //BEZIER_H