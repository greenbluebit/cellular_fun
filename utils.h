int clamp(int d, int min, int max) {
  const int t = d < min ? min : d;
  return t > max ? max : t;
}