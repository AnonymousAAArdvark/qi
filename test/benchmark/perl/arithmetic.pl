use strict;
use warnings;
use v5.10;

use Time::HiRes qw(clock);

my $start = clock();

my $result = 0;
for (my $i = 0; $i < 10000000; $i++) {
    $result = $result + 11;
    $result = $result * 10;
    $result = $result - ($result / 100) * 99;
}

say $result;
say clock() - $start;