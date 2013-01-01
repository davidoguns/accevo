#! /usr/bin/perl

use strict;
use warnings;

my $targetDir = shift;

my $cmd = "cat $targetDir\* | wc -l";

