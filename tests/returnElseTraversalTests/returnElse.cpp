int main() {
  int Signed = 1;
  for (; Signed < 2; Signed++) {
    if (Signed) {
      if (1) {
        return 2; // violation #1
      } else {
        break;    // violation #2
      }
    } else {
      if (2) {
        return 3; // violation #3
      } else {
        break;    // OK, does not appear before an else
      }
    }
  }
}
