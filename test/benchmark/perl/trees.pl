use strict;
use warnings;
use v5.10;

use Time::HiRes qw(clock);

{
    package Tree;

    sub new {
      my $class = shift;
      my $depth = shift;
      my $self = { depth => $depth };

      if ($depth > 0) {
        $self->{a} = Tree->new($depth - 1);
        $self->{b} = Tree->new($depth - 1);
        $self->{c} = Tree->new($depth - 1);
        $self->{d} = Tree->new($depth - 1);
        $self->{e} = Tree->new($depth - 1);
      }

      bless $self => $class;
      return $self;
    }

    sub walk {
      my $self = shift;

      if ($self->{depth} == 0) {
        return 0
      }

      return $self->{depth} 
        + $self->{a}->walk()
        + $self->{b}->walk()
        + $self->{c}->walk()
        + $self->{d}->walk()
        + $self->{e}->walk();
    }
}

my $tree = Tree->new(8);
my $start = clock();

for (my $i = 0; $i < 100; $i++) {
  if ($tree->walk() != 122068) {
    say "Error";
  } 
}
say clock() - $start;
