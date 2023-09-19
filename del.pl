use strict;
use warnings;

my $ARGC = @ARGV;

unless ($ARGC) {
    `rm join/*  -r 2> /dev/null`;
    `rm table/* -r 2> /dev/null`;
}
else {
    foreach my $name (@ARGV) {
        `rm join/*.$name.*  -r 2> /dev/null`;
        `rm table/*.$name.* -r 2> /dev/null`;
}   }