use strict;
use warnings;
use v5.10;

use Time::HiRes qw(clock);

{
    package Foo;

    sub new {
      my $class = shift;
      my $self = {
        field0 => 1,
        field1 => 1,
        field2 => 1,
        field3 => 1,
        field4 => 1,
        field5 => 1,
        field6 => 1,
        field7 => 1,
        field8 => 1,
        field9 => 1,
        field10 => 1,
        field11 => 1,
        field12 => 1,
        field13 => 1,
        field14 => 1,
        field15 => 1,
        field16 => 1,
        field17 => 1,
        field18 => 1,
        field19 => 1,
        field20 => 1,
        field21 => 1,
        field22 => 1,
        field23 => 1,
        field24 => 1,
        field25 => 1,
        field26 => 1,
        field27 => 1,
        field28 => 1,
        field29 => 1,
      };

      bless $self => $class;
      return $self;
    }

    sub method0 { return $_[0]->{field0}; }
    sub method1 { return $_[0]->{field1}; }
    sub method2 { return $_[0]->{field2}; }
    sub method3 { return $_[0]->{field3}; }
    sub method4 { return $_[0]->{field4}; }
    sub method5 { return $_[0]->{field5}; }
    sub method6 { return $_[0]->{field6}; }
    sub method7 { return $_[0]->{field7}; }
    sub method8 { return $_[0]->{field8}; }
    sub method9 { return $_[0]->{field9}; }
    sub method10 { return $_[0]->{field10}; }
    sub method11 { return $_[0]->{field11}; }
    sub method12 { return $_[0]->{field12}; }
    sub method13 { return $_[0]->{field13}; }
    sub method14 { return $_[0]->{field14}; }
    sub method15 { return $_[0]->{field15}; }
    sub method16 { return $_[0]->{field16}; }
    sub method17 { return $_[0]->{field17}; }
    sub method18 { return $_[0]->{field18}; }
    sub method19 { return $_[0]->{field19}; }
    sub method20 { return $_[0]->{field20}; }
    sub method21 { return $_[0]->{field21}; }
    sub method22 { return $_[0]->{field22}; }
    sub method23 { return $_[0]->{field23}; }
    sub method24 { return $_[0]->{field24}; }
    sub method25 { return $_[0]->{field25}; }
    sub method26 { return $_[0]->{field26}; }
    sub method27 { return $_[0]->{field27}; }
    sub method28 { return $_[0]->{field28}; }
    sub method29 { return $_[0]->{field29}; }
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
