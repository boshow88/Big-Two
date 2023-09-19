use strict;
use warnings;
use Time::HiRes qw(time);

my $now    = $ARGV[0];
my $player = $ARGV[1];
my $name   = "$now.$player";
my ($host, $table);

START:

if (`find join/*.host 2>&1` =~ /No such file or directory/) {
    `touch join/$name.host`;
    goto HOST;
}
else {
    `touch join/$name.join`;
    goto JOIN;
}

HOST:

my @joins = ();
while ($#joins < 2) {
    if (`find join/*.host 2>&1` =~ /^join\/(.+)\.host/ and $1 ne $name) {
        `mv join/$name.host join/$name.join`;
        # print "\33[1;6H->Join\n";
        goto JOIN;
    }
    @joins = `find join/*.join 2>&1` =~ /join\/(.+)\.join\n/g;
    open  TIME, ">join/$name.time";
    print TIME time()."\n";
    close TIME;
}

@joins = @joins[0 .. 2];
my %is_joined = ($joins[0]=>'no', $joins[1]=>'no', $joins[2]=>'no');
foreach my $join_ (@joins) {
    `rm table/$join_.play 2>&1`;
}
open  CHECK, ">join/$name.host";
print CHECK join("\n", @joins)."\n";
close CHECK;

my $is_all_checked = 'no';
for (my $check_in_time = time() + 3; $is_all_checked eq 'no' and time() < $check_in_time; ) {
    $is_all_checked = 'yes';
    foreach my $join_ (@joins) {
        if (`find table/$join_.play 2>&1` =~ /No such file or directory/) {
            $is_all_checked = 'no';
            $is_joined{$join_} = 'no';
        }
        else {
            $is_joined{$join_} = 'yes';
        }
    }
}

if ($is_all_checked eq 'yes') {
    $host = $name;
    goto END;
}
else {
    foreach my $join_ (@joins) {
        if ($is_joined{$join_} eq 'no') {
            `rm join/$join_.join`;
    }   }
    goto HOST;
}

JOIN:

for (my $success_joined = 'no'; $success_joined eq 'no'; ) {
    foreach my $host_ (`find join/*.host 2>&1` =~ /join\/(.+)\.host\n/g) {
        my $cat_host = `cat join/$host_.host 2>&1`;
        my $cat_time = `cat join/$host_.time 2> /dev/null`;
        if ($cat_time =~ /^(.+)\n/ and time() > $1 + 5.0) {
            `rm join/$host_.host 2> /dev/null`;
            `rm join/$host_.time 2> /dev/null`;
            `rm join/$name.join 2>&1`;
            goto START;
        }
        if ($cat_host =~ /$name\n/) {
            $host = $host_;
            $success_joined = 'yes';
            last;
        }
    }
}

END:

if ($name eq $host) {
    open  TABLE, ">table/$name.play";
    print TABLE "$name\nhost";
    close TABLE;
    $table = "table/$name.table";
    `mv join/$name.host $table 2>&1`;
    `rm join/$name.time 2>&1`;
}
else {
    while (1) {
        if (`find table/$name.play 2>&1` =~ /No such file or directory/) {
            open  TABLE, ">table/$name.play";
            print TABLE $host;
            close TABLE;
            $table = "table/$host.table";
        }
        `find $table 2>&1` =~ /No such file or directory/ or last;
    }
    `rm join/$name.join 2>&1`;
}