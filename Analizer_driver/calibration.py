
class Point:
    def __init__(self, x, y):
        self._x = x
        self._y = y

    @property
    def x(self):
        return self._x

    @property
    def y(self):
        return self._y


class CalCurve:
    _points = [] # [point, ...]

    # FIXME: it is assumed that x is increasing each point!
    def x_to_y(self, x) -> float:
        if len(self.points) < 2:
            return 0 # unknown slope. we need at least 2 points!
        point1: Point = self.points[0]
        point2: Point = self.points[1]

        # check if we are below the range
        for i in range(len(self.points) - 1):
            point1 = self.points[i]
            point2 = self.points[i + 1]
            if x <= point2.x:
                break

        # y = mx + b # linear interpolation
        return ((x - point1.x) * (point2.y - point1.y)) / (point2.x - point1.x) + point1.y

    # FIXME: it is assumed that y is increasing each point!
    def y_to_x(self, y) -> int:
        if len(self.points) < 2:
            return 0 # unknown slope. we need at least 2 points!
        point1: Point = self.points[0]
        point2: Point = self.points[1]

        # check if we are below the range
        for i in range(len(self.points) - 1):
            point1 = self.points[i]
            point2 = self.points[i + 1]
            if y <= point2.y:
                break

        # x = my + b # linear interpolation
        return ((y - point1.y) * (point2.x - point1.x)) / (point2.y - point1.y) + point1.x

    @property
    def points(self):
        return self._points

    @points.setter
    def points(self, points):
        self._points = points
