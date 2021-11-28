use strict;
use warnings;
use v5.10;

use Time::HiRes qw(clock);

{
  package Tree;

  sub new {
    my $class = shift;
    my $item = shift;
    my $depth = shift;

    my $self = {
      item => $item,
      depth => $depth
    };

    if ($depth > 0) {
      my $item2 = $item + $item;
      $depth = $depth - 1;
      $self->{left} = Tree->new($item2 - 1, $depth);
      $self->{right} = Tree->new($item2, $depth);
    } else {
      $self->{left} = undef;
      $self->{right} = undef;
    }

    bless $self => $class;
    return $self;
  }

  sub check {
    my $self = shift;

    if (!defined $self->{left}) {
      return $self->{item};
    }

    return $self->{item} + $self->{left}->check() - $self->{right}->check();
  }
}

my $minDepth = 4;
my $maxDepth = 14;
my $stretchDepth = $maxDepth + 1;

my $start = clock();

say "stretch tree of depth:";
say $stretchDepth;
say "check:";
say Tree->new(0, $stretchDepth)->check();

my $longLivedTree = Tree->new(0, $maxDepth);

# iterations = 2 ** $maxDepth
my $iterations = 1;
my $d = 0;
while ($d < $maxDepth) {
  $iterations = $iterations * 2;
  $d++;
}

my $depth = $minDepth;
while ($depth < $stretchDepth) {
  my $check = 0;
  my $i = 1;
  while ($i <= $iterations) {
    $check = $check + Tree->new($i, $depth)->check() + Tree->new(-$i, $depth)->check();
    $i++;
  }

  say "num trees:";
  say $iterations * 2;
  say "depth:";
  say $depth;
  say "check:";
  say $check;

  $iterations = $iterations / 4;
  $depth = $depth + 2;
}

say "long lived tree of depth:";
say $maxDepth;
say "check:";
say $longLivedTree->check();
say "elapsed:";
say clock() - $start;
