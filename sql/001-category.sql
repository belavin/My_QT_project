create table category (
   iid serial not null primary key ,
   rid_parent    integer references category(iid),
   icon          bytea ,
   title         text not null unique ,
   registered_at timestamp not null default CURRENT_TIMESTAMP,
   fast          boolean ,
   stat_code     varchar(10) ,
   locked_at     timestamp ,
   acomment      text
) ;