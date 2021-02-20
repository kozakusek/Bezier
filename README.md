# Bezier

A library for plotting [cubic Bezier curves](https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves).  

### Quick description

 + `Cup()` – U-shaped Bezier curve. Control points:
           p0 = (-1, 1), p1 = (-1, -1), p2 = (1, -1) i p3 = (1, 1);
 + `Cap()` – Cup() rotated by 180 degrees. Control points:
           p0 = (-1, -1), p1 = (-1, 1), p2 = (1, 1) i p3 = (1, -1);
 + `ConvexArc()` –  Convex quarter of unit circle. Control points:
                 p0 = (0, 1), p1 = (ARC, 1), p2 = (1, ARC) i p3 = (1, 0),
                 where ARC = 4*(sqrt(2) - 1)/3;
 + `ConcaveArc()` – Concave quarter of unit circle. Control points:
                  p0 = (0, 1), p1 = (0, 1 - ARC), p2 = (1 - ARC, 0) i p3 = (1, 0);
 + `LineSegment(p, q)` – A segment between p and q. Control points:
                  p0 = p1 = p i p2 = p3 = q;
                  
 + `MovePoint(f, i, x , y)` – translates i-th node of f by x along X axis
                            and by y along Y axis;
 + `Rotate(f, a)` – rotates all nodes of curve f around the point (0,0) by a degrees counterclockwise;
 + `Scale(f, x, y)` – stretches the nodes of f in respect to (0, 0) by x
                    along X axis and by y along Y axis;
 + `Translate(f, x, y)` – transaltes all nodes of curve f by x along X axis
                        and by y along Y axis;

+ `Print(s, fb, bg)` – prints to the output stream s the curve described in constructor
                       using fb for the curve and bg for the background 
                       default values: s - std::cout, fg - '*', bg - ' ';
 + `operator()` – returns B(t).
                
 ### Examples
 
 A few examples are in the file `bezier_example.c`.
 
 ![alt text](https://github.com/kozakusek/pictures/blob/main/bezier.png?raw=true "Examples")
