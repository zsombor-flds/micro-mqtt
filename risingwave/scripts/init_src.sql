--drop table test
create table test
(
    "timestamp" timestamp,
    temperature float,
    humidity float
)
with (
    connector = 'mqtt',
    url = 'tcp://mqtt:1883',
    topic = 'raw/sensor',
    qos = 'at_least_once',
   username = 'test',
   password = 'test'
)
format plain encode json;


select
    *
from test
