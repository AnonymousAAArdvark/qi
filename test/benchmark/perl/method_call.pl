use strict;
use warnings;
use v5.10;

use Time::HiRes qw(clock);

{
    package Toggle;

    sub new {
      my $class = shift;
      my $startState = shift;

      my $self = {
        state => $startState
      };

      bless $self => $class;
      return $self;
    }

    sub value {
      my $self = shift;
      return $self->{state};
    }

    sub activate {
      my $self = shift;
      $self->{state} = !$self->{state};
      return $self;
    }

    1;

    package NthToggle;
    our @ISA = qw(Toggle);

    sub new {
      my $class = shift;
      my $startState = shift;
      my $maxCounter = shift;

      my $self = $class->SUPER::new($startState);
      $self->{countMax} = $maxCounter;
      $self->{count} = 0;

      bless $self => $class;
      return $self;
    }

    sub activate {
      my $self = shift;
      $self->{count}++;
      if ($self->{count} >= $self->{countMax}) {
        $self->SUPER::activate();
        $self->{count} = 0;
      }

      return $self;
    }
}

my $start = clock();
my $n = 100000;
my $val = 1;
my $toggle = Toggle->new($val);

for (my $i = 0; $i < $n; $i++) {
  $val = $toggle->activate()->value();
  $val = $toggle->activate()->value();
  $val = $toggle->activate()->value();
  $val = $toggle->activate()->value();
  $val = $toggle->activate()->value();
  $val = $toggle->activate()->value();
  $val = $toggle->activate()->value();
  $val = $toggle->activate()->value();
  $val = $toggle->activate()->value();
  $val = $toggle->activate()->value();
}

say $toggle->value();

$val = 1;
my $ntoggle = NthToggle->new($val, 3);

for (my $i = 0; $i < $n; $i++) {
  $val = $ntoggle->activate()->value();
  $val = $ntoggle->activate()->value();
  $val = $ntoggle->activate()->value();
  $val = $ntoggle->activate()->value();
  $val = $ntoggle->activate()->value();
  $val = $ntoggle->activate()->value();
  $val = $ntoggle->activate()->value();
  $val = $ntoggle->activate()->value();
  $val = $ntoggle->activate()->value();
  $val = $ntoggle->activate()->value();
}
say $ntoggle->value();
say clock() - $start;
