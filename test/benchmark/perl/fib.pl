use strict;
use warnings;
use v5.10;

use Time::HiRes qw(clock);

sub fib {
  my $n = shift; 
  if ($n < 2) {
    return $n;
  }
  return fib($n - 2) + fib($n - 1);
}

my $start = clock();
say fib(35) == 9227465;
say clock() - $start;
