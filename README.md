# Bezier

A library for plotting [cubic Bezier curves](https://en.wikipedia.org/wiki/B%C3%A9zier_curve#Cubic_B%C3%A9zier_curves).  

### Quick description

 + `Cup()` – zwraca funkcję generującą punkty kontrolne
           p0 = (-1, 1), p1 = (-1, -1), p2 = (1, -1) i p3 = (1, 1),
           definiujące krzywą Béziera w kształcie litery U;
 + `Cap()` – zwraca funkcję generującą punkty kontrolne
           p0 = (-1, -1), p1 = (-1, 1), p2 = (1, 1) i p3 = (1, -1),
           dające krzywą w kształcie litery U obróconej o 180 stopni;
 + `ConvexArc()` –  zwraca funkcję generującą węzły
                 p0 = (0, 1), p1 = (ARC, 1), p2 = (1, ARC) i p3 = (1, 0),
                 gdzie ARC = 4*(sqrt(2) - 1)/3, tworzące łuk wypukły zbliżony
                 do ćwiartki okręgu jednostkowego;
 + `ConcaveArc()` – zwraca funkcję generującą węzły
                  p0 = (0, 1), p1 = (0, 1 - ARC), p2 = (1 - ARC, 0) i p3 = (1, 0),
                  gdzie ARC j.w., tworzące łuk wklęsły zbliżony do ćwiartki
                  okręgu;
 + `LineSegment(p, q)` – zwraca funkcję generującą węzły p0 = p1 = p i p2 = p3 = q,
                       definiujące odcinek prostej łączący punkty p i q.
                       
 + `MovePoint(f, i, x , y)` – przesuwa i-ty węzeł krzywej f o x wzdłuż osi X
                            i o y wzdłuż osi Y;
 + `Rotate(f, a)` – obraca wszystkie węzły krzywej f wokół punktu (0, 0) o kąt
                  a wyrażony w stopniach, przeciwnie do ruchu wskazówek zegara;
 + `Scale(f, x, y)` – skaluje węzły krzywej f względem punktu (0, 0) skalą x
                    wzdłuż osi X i skalą y wzdłuż osi Y;
 + `Translate(f, x, y)` – przesuwa wszystkie węzły krzywej f o x wzdłuż osi X
                        i o y wzdłuż osi Y.
                        
+ `Print(s, fb, bg)` – funkcja stała, drukująca do strumienia wyjściowego s
                      (domyślnie std::cout) kwadrat o boku rozdzielczość,
                      zbudowany ze znaków tak, że punkty leżące na krzywej
                      drukowane są znakiem fb (domyślnie '*'), zaś punkty tła
                      drukowane są znakiem bg (domyślnie spacja);
 + `operator()` – funkcja stała, pobierająca kolejno w parametrach funkcję
                dostarczającą węzły krzywej, parametr zmiennoprzecinkowy t oraz
                numer segmentu (licząc od 1); celem tego operatora jest
                zwrócenie punktu B(t) leżącego na wskazanym segmencie Béziera.
                
 ### Examples
 
 A few examples are in the file `bezier_example.c`.
 
 ![alt text](https://github.com/kozakusek/pictures/blob/main/bezier.png?raw=true "Examples")
