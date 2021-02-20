#ifndef JNP1_Z7_BEZIER_H
#define JNP1_Z7_BEZIER_H

#include <cmath>
#include <iostream>
#include <tuple>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

namespace bezier::types {
    using node_index_t = std::size_t;
    using real_t = double;

    struct point_2d {
        const real_t X;
        const real_t Y;

        point_2d() = delete;

        point_2d(real_t x, real_t y) : X(x), Y(y) {}

        point_2d operator+(point_2d b) const {
            return point_2d(this->X + b.X, this->Y + b.Y);
        }

        point_2d operator*(real_t k) const {
            return point_2d(k * this->X, k * this->Y);
        }

        bool operator==(point_2d p) const {
            return p.X == this->X && p.Y == this->Y;
        }
    };

    inline point_2d operator*(const real_t k, const point_2d &p) {
        return p * k;
    }

    inline std::ostream &operator<<(std::ostream &stream, const point_2d &a) {
        return stream << "(" << a.X << ", " << a.Y << ")";
    }
}

namespace bezier::constants {
    constexpr types::node_index_t NUM_OF_CUBIC_BEZIER_NODES = 4;
    const types::real_t ARC = 4 * (std::sqrt(2) - 1) / 3;
}

// A homemade match notation is used in this code.
// It uses "? :" operator and sometimes may look like an unnecessarily line-broken "? :" statement,
// but it is only to underline the fact that it is used as a "match" expression.

namespace bezier::detail {
    using res_t = int;
    using pixel_t = std::tuple<int, int>;
    using fpoint_t = std::function<const types::point_2d(const types::node_index_t)>;
    using points_t = std::tuple<types::point_2d, types::point_2d, types::point_2d, types::point_2d>;
    using matrix_t = std::tuple<types::real_t, types::real_t, types::real_t, types::real_t>;
    // We use indexing in matrix_t this way:
    //  [ 0 , 1 ]
    //  [ 2 , 3 ]

    inline types::point_2d multiply(matrix_t m, const types::point_2d &p) {
        return types::point_2d(
                p.X * std::get<0>(m) + p.Y * std::get<1>(m),
                p.X * std::get<2>(m) + p.Y * std::get<3>(m)
        );
    }

    // An order used in the List.
    inline bool operator<(const pixel_t &a, const pixel_t &b) {
        return std::get<1>(a) > std::get<1>(b) || (std::get<1>(a) == std::get<1>(b) && std::get<0>(a) < std::get<0>(b));
    }

    // To allow for more 'functional' flexibility - a sorted linked-list.
    class ListNode;
    using List = std::shared_ptr<const ListNode>;

    class ListNode {
        const pixel_t el;
        const std::size_t r_length;
        const List left, right;

    public:
        explicit ListNode(const pixel_t &point) : ListNode(point, 1, nullptr, nullptr) {}

        ListNode(pixel_t point, std::size_t r_length, List left, List right)
                : el(std::move(point)), left(std::move(left)), right(std::move(right)), r_length(r_length) {}

    private:
        friend inline std::size_t getRLength(const List &);

        friend inline List joinLists(const List &left, const List &right);

        friend inline List addToList(const List &list, const pixel_t &point);

        friend inline pixel_t getHead(const List &);

        friend inline List removeHead(const List &);
    };

    inline bool isEmpty(const List &list) {
        return list == nullptr;
    }

    template<typename... Args>
    inline List makeList(Args &&... args) {
        return std::make_shared<const ListNode>(args...);
    }

    inline std::size_t getRLength(const List &list) {
        return isEmpty(list) ? 0 : list->r_length;
    }

    inline List joinLists(const List &left, const List &right) {
        List temp;
        return
                (isEmpty(left)) ? right :
                (isEmpty(right)) ? left :
                (right->el < left->el) ? joinLists(right, left) :
                (getRLength(temp = joinLists(left->right, right)) < getRLength(left->left)) ?
                makeList(left->el, getRLength(temp) + 1, left->left, temp) :
                makeList(left->el, getRLength(left->left) + 1, temp, left->left);
    }

    inline List addToList(const List &list, const pixel_t &point) {
        return joinLists(list, makeList(point));
    }

    inline pixel_t getHead(const List &list) {
        return list->el;
    }

    inline List removeHead(const List &list) {
        return joinLists(list->left, list->right);
    }

    inline List removeSmaller(const List &list, const pixel_t &pixel) {
        return
                (isEmpty(list)) ? nullptr :
                (getHead(list) < pixel || getHead(list) == pixel) ? removeSmaller(removeHead(list), pixel) :
                list;
    }

    // Auxiliary function which creates a segment (== function) from given 4-tuple of points.
    // Returned function may throw an "out_of_range" exception.
    inline fpoint_t makeSegment(detail::points_t points) {
        return [=](types::node_index_t idx) -> types::point_2d {
            return
                    (idx == 0) ? std::get<0>(points) :
                    (idx == 1) ? std::get<1>(points) :
                    (idx == 2) ? std::get<2>(points) :
                    (idx == 3) ? std::get<3>(points) :
                    throw std::out_of_range("a curve node index is out of range");
        };
    }

    inline types::real_t toRadians(types::real_t a) {
        return a * M_PI / 180.;
    }
}

namespace bezier {
    class P3CurvePlotter {
    private:
        const detail::res_t resolution;
        const detail::List list;

    public:
        explicit P3CurvePlotter(const detail::fpoint_t &f, types::node_index_t segments = 1, detail::res_t res = 80)
                : resolution(res),
                  list(buildList(f, 1, segments, segments / static_cast<types::real_t>(resolution * resolution),
                                 nullptr, resolution)) {}

        void Print(std::ostream &s = std::cout, char fb = '*', char bg = ' ') const {
            return print_aux(s, fb, bg, 0, resolution - 1, list);
        }

        types::point_2d operator()(const detail::fpoint_t &f, types::real_t t, types::node_index_t idx) const {
            return casteljau(t, getSegmentPoints(f, idx));
        }

    private:
        static inline types::node_index_t get_fst_idx(types::node_index_t seg_idx) {
            return (seg_idx - 1) * constants::NUM_OF_CUBIC_BEZIER_NODES;
        }

        [[nodiscard]] static inline detail::points_t getSegmentPoints(const detail::fpoint_t &f, types::node_index_t idx) {
            return {
                    f(get_fst_idx(idx)),
                    f(get_fst_idx(idx) + 1),
                    f(get_fst_idx(idx) + 2),
                    f(get_fst_idx(idx) + 3),
            };
        }

        // Recursively performs the deCasteljau algorithm.
        // Uses a helper variable - iter, which represents which step is being now calculated.
        [[nodiscard]] static inline types::point_2d casteljau(const types::real_t t, const detail::points_t &points,
                                                       int iter = -1) {
            return
                    (iter == 0) ? (1 - t) * std::get<0>(points) + t * std::get<1>(points) :
                    (iter == 1) ? (1 - t) * std::get<1>(points) + t * std::get<2>(points) :
                    (iter == 2) ? (1 - t) * std::get<2>(points) + t * std::get<3>(points) :
                    (iter == 3) ? (1 - t) * casteljau(t, points, 0) + t * casteljau(t, points, 1) :
                    (iter == 4) ? (1 - t) * casteljau(t, points, 1) + t * casteljau(t, points, 2) :
                    (1 - t) * casteljau(t, points, 3) + t * casteljau(t, points, 4);
        }

        static inline detail::pixel_t getPixel(types::real_t t, const detail::points_t &points, detail::res_t res) {
            return detail::pixel_t(
                    std::floor((casteljau(t, points).X + 1) * res / 2),
                    std::floor((casteljau(t, points).Y + 1) * res / 2)
            );
        }

        [[nodiscard]] inline detail::List
        buildList(const detail::fpoint_t &f, types::node_index_t idx, types::node_index_t segments,
                  types::real_t eps, const detail::List &lst, detail::res_t res) const {
            return (idx > segments) ? lst :
                   buildList(f, idx + 1, segments, eps,
                             joinLists(lst,
                                       buildList_aux(f, getSegmentPoints(f, idx),
                                                     0, nullptr, eps, res)
                             ), res
                   );
        }

        [[nodiscard]] inline detail::List buildList_aux(const detail::fpoint_t &f, const detail::points_t &points,
                                                 types::real_t t, const detail::List &lst, types::real_t eps,
                                                 detail::res_t res) const {
            return (t > 1) ? lst : buildList_aux(f, points, t + eps,
                                                 isEmpty(lst) ? detail::makeList(getPixel(t, points, res)) :
                                                 (getHead(lst) == getPixel(t, points, res)) ? lst :
                                                 addToList(lst, getPixel(t, points, res)),
                                                 eps, res);
        }

        inline void print_aux(std::ostream &s, char fb, char bg, detail::res_t x, detail::res_t y,
                       const detail::List &lst) const {
            return
                    (y < 0) ? void() :
                    (x == resolution) ? print_aux(s << '\n', fb, bg, 0, y - 1, lst) :
                    print_aux(s << (isEmpty(lst) || !(getHead(lst) == detail::pixel_t(x, y)) ? bg : fb),
                              fb, bg, x + 1, y, detail::removeSmaller(lst, detail::pixel_t(x, y)));
        }
    };

    inline detail::fpoint_t Concatenate(const detail::fpoint_t &f1, const detail::fpoint_t &f2) {
        return [=](types::node_index_t idx) -> types::point_2d {
            return
                    (idx < constants::NUM_OF_CUBIC_BEZIER_NODES) ? f1(idx) :
                    f2(idx - constants::NUM_OF_CUBIC_BEZIER_NODES);
        };
    }

    template<typename... Args>
    inline detail::fpoint_t Concatenate(const detail::fpoint_t &f1, Args... nodes) {
        return Concatenate(f1, Concatenate(nodes...));
    }

    inline detail::fpoint_t Cup() {
        return detail::makeSegment({
                                           types::point_2d(-1, 1),
                                           types::point_2d(-1, -1),
                                           types::point_2d(1, -1),
                                           types::point_2d(1, 1)
                                   });
    }

    inline detail::fpoint_t Cap() {
        return detail::makeSegment({
                                           types::point_2d(-1, -1),
                                           types::point_2d(-1, 1),
                                           types::point_2d(1, 1),
                                           types::point_2d(1, -1)
                                   });
    }

    inline detail::fpoint_t ConvexArc() {
        return detail::makeSegment({
                                           types::point_2d(0, 1),
                                           types::point_2d(constants::ARC, 1),
                                           types::point_2d(1, constants::ARC),
                                           types::point_2d(1, 0)
                                   });
    }

    inline detail::fpoint_t ConcaveArc() {
        return detail::makeSegment({
                                           types::point_2d(0, 1),
                                           types::point_2d(0, 1 - constants::ARC),
                                           types::point_2d(1 - constants::ARC, 0),
                                           types::point_2d(1, 0)
                                   });
    }

    inline detail::fpoint_t LineSegment(types::point_2d p, types::point_2d q) {
        return detail::makeSegment({p, p, q, q});
    }

    inline detail::fpoint_t MovePoint(const detail::fpoint_t &f, types::node_index_t i, types::real_t x, types::real_t y) {
        return [=](types::node_index_t idx) -> types::point_2d {
            return
                    (i == idx) ? types::point_2d(x, y) + f(idx) :
                    f(idx);
        };
    }

    inline detail::fpoint_t Rotate(const detail::fpoint_t &f, types::real_t a) {
        return [=](types::node_index_t idx) -> types::point_2d {
            return detail::multiply({std::cos(detail::toRadians(a)), -std::sin(detail::toRadians(a)),
                                     std::sin(detail::toRadians(a)), std::cos(detail::toRadians(a))
                                    }, f(idx));
        };
    }

    inline detail::fpoint_t Scale(const detail::fpoint_t &f, types::real_t x, types::real_t y) {
        return [=](types::node_index_t idx) -> types::point_2d {
            return detail::multiply({x, 0, 0, y}, f(idx));
        };
    }

    inline detail::fpoint_t Translate(const detail::fpoint_t &f, types::real_t x, types::real_t y) {
        return [=](types::node_index_t idx) -> types::point_2d {
            return types::point_2d(x, y) + f(idx);
        };
    }
}

#endif //JNP1_Z7_BEZIER_H
