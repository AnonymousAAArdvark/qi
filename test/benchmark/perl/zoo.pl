use strict;
use warnings;
use v5.10;

use Time::HiRes qw(clock);

{
    package Zoo;

    sub new {
      my $class = shift;
      my $self = {
        aarvark  => 1,
        baboon   => 1,
        cat      => 1,
        donkey   => 1,
        elephant => 1,
        fox      => 1,
      };
      bless $self => $class;
      return $self;
    }

    sub ant    { $_[0]->{aarvark}; }
    sub banana { $_[0]->{baboon}; }
    sub tuna   { $_[0]->{cat}; }
    sub hay    { $_[0]->{donkey}; }
    sub grass  { $_[0]->{elephant}; }
    sub mouse  { $_[0]->{fox}; }
}

my $zoo = Zoo->new();
my $sum = 0;
my $start = clock();
while ($sum < 10000000) {
  $sum = $sum + $zoo->ant()
            + $zoo->banana()
            + $zoo->tuna()
            + $zoo->hay()
            + $zoo->grass()
            + $zoo->mouse();
}

say $sum;
say clock() - $start;
