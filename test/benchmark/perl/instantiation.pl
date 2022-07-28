use strict;
use warnings;
use v5.10;

use Time::HiRes qw(clock);

{
    package Foo;

    sub new {
      my $class = shift;
      my $self = {};
      bless $self => $class;
      return $self;
    }
}

my $start = clock();
my $i = 0;
while ($i < 500000) {
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  Foo->new();
  $i++;
}

say clock() - $start;
