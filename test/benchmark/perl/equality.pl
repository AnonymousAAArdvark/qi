use strict;
use warnings;
use v5.10;

use Time::HiRes qw(clock);

  no warnings qw(void uninitialized);

my $i = 0;
my $loopStart = clock();

while ($i < 10000000) {
  $i = $i + 1;

  1; 1; 1; 2; 1; undef; 1; "str"; 1; 1;
  undef; undef; undef; 1; undef; "str"; undef; 1;
  1; 1; 1; 1; 1; 0; 1; "str"; 1; undef;
  "str"; "str"; "str"; "stru"; "str"; 1; "str"; undef; "str"; 1;
}

my $loopTime = clock() - $loopStart;

my $start = clock();

$i = 0;
while ($i < 10000000) {
  $i = $i + 1;

  1 == 1; 1 == 2; 1 eq undef; 1 eq "str"; 1 == 1;
  undef eq undef; undef == 1; undef eq "str"; undef == 1;
  1 == 1; 1 == 1; 1 == 0; 1 eq "str"; 1 eq undef;
  "str" eq "str"; "str" eq "stru"; "str" eq 1; "str" eq undef; "str" eq 1;
}

my $elapsed = clock() - $start;
say "loop";
say $loopTime;
say "elapsed";
say $elapsed;
say "equals";
say $elapsed - $loopTime;
