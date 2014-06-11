int main() {
  int a = 0;
  int b = 1;

  // OKAY
  if (1) {
    a += b;
    a += b;
    a += b;
    a += b;
    a += b;
  }

  // Candidate (standard)
  // 10 statements *not counting if statement)
  if (1) {  // violation #1
    a += b;
    a += b;
    a += b;
    a += b;
    a += b;
    a += b;
    a += b;
    a += b;
    a += b;
    a += b;
  }

  // Candidate (nested)
  // 21 statements (not counting top-level if statement)
  #pragma acre ignore
  if (1) {  // violation #2
    a += b;
    a += b;
    a += b;
    a += b;
    a += b;

    // 15 statements (not counting if statement)
    if (1) {  // technically a violation, but suppressed due to being nested
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
      a += b;
    }
  }
  return 0;
}
