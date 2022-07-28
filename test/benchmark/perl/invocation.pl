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

    sub method0 { }
    sub method1 { }
    sub method2 { }
    sub method3 { }
    sub method4 { }
    sub method5 { }
    sub method6 { }
    sub method7 { }
    sub method8 { }
    sub method9 { }
    sub method10 { }
    sub method11 { }
    sub method12 { }
    sub method13 { }
    sub method14 { }
    sub method15 { }
    sub method16 { }
    sub method17 { }
    sub method18 { }
    sub method19 { }
    sub method20 { }
    sub method21 { }
    sub method22 { }
    sub method23 { }
    sub method24 { }
    sub method25 { }
    sub method26 { }
    sub method27 { }
    sub method28 { }
    sub method29 { } 
}


my $foo = Foo->new();
my $start = clock();
my $i = 0;
while ($i < 500000) {
  $foo->method0();
  $foo->method1();
  $foo->method2();
  $foo->method3();
  $foo->method4();
  $foo->method5();
  $foo->method6();
  $foo->method7();
  $foo->method8();
  $foo->method9();
  $foo->method10();
  $foo->method11();
  $foo->method12();
  $foo->method13();
  $foo->method14();
  $foo->method15();
  $foo->method16();
  $foo->method17();
  $foo->method18();
  $foo->method19();
  $foo->method20();
  $foo->method21();
  $foo->method22();
  $foo->method23();
  $foo->method24();
  $foo->method25();
  $foo->method26();
  $foo->method27();
  $foo->method28();
  $foo->method29();
  $i++;
}

say clock() - $start;
