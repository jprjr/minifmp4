#!/usr/bin/env perl

use strict;
use warnings;

my $includes = {};


open(my $fh, '>:encoding(utf8)', 'minifmp4.h');

sub handle_include {
    my $file = shift;
    return if exists($includes->{$file});

    open(my $sub_fh, '<:encoding(utf8)', 'src/'.$file) or die "unable to open $file: $!";
    my @lines = <$sub_fh>;
    close($sub_fh);

    # remove ifdef guards
    if($file =~ m/\.h$/) {
        shift(@lines);
        shift(@lines);
        pop(@lines);
    }

    foreach my $line (@lines) {
        my ($inc) = ($line =~ m/^#include\s+"([^"]+)"/);
        if(not defined($inc)) {
            print $fh $line;
            next;
        }
        handle_include($inc);
    }
    $includes->{$file} = 1;
}

open(my $h_fh, '<:encoding(utf8)', 'src/minifmp4.h');
while(my $line = <$h_fh>) {
    my ($inc) = ($line =~ m/^#include\s+"([^"]+)"/);
    if(not defined($inc)) {
        print $fh $line;
        next;
    }
    handle_include($inc);
}
close($h_fh);
close($fh);
