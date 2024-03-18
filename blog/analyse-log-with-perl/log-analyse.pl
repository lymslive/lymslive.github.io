#! /usr/bin/env perl
# Usage: ./log-analyse.pl *.log
use strict;
use warnings;

my $pattern = 'context sentence with key: (\S+)';
my $log = {};

while (<>) {
	chomp;
	if (/$pattern/) {
		my $key = $1;
		++$log->{$key};
	}
}

foreach my $key (keys %$log) {
	print "$key\t$log->{$key}\n";
}

