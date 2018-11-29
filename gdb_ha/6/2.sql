-- a)

create table user (
	id integer not null unique,
	name varchar(30) not null,
	primary key (id)
);

create table tweet (
	id integer not null unique,
	user_id integer not null,
	date date not null;
	text stirng primary key (id)
);

create table user (
	id integer not null unique,
	name varchar(30) not null,
	primary key(id)
);

-- b)

create table user (
	id integer not null unique,
	name varchar(30) not null,
	primary key (id)
);

create table tweet (
	id integer not null unique,
	user_id integer unique references user on delete set null,
	date date not null;
	text string
	primary key (id)
);

create table follows (
	follower_id integer unique,
	follows_id integer unique
	primary key (followr_id, follows_id) references user on delete set null
);

create table likes (
	user_id integer unique,
	tweet_id integer unique,
	date date not null
	primary key (user_id, tweet_id) references user tweet on delete set null
);

-- c)

te table user (
	id integer not null unique,
	name varchar(30) not null,
	primary key(id)
);

create table tweet (
	id integer not null unique,
	user_id integer unique references user on delete set null,
	date date not null,
	text string references user on delete replace(text, text, 'removed')
	primary key (id)
);
