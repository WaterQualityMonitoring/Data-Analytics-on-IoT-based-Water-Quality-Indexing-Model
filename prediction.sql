---------dataset
SELECT DATE(Created_at) AS date_time, AVG(WQI) AS WQI_final
FROM `Water.water_data`
GROUP BY date_time;


--------creating model
create or replace model
`Water.forecast_wqi`
options(MODEL_TYPE='ARIMA',
TIME_SERIES_TIMESTAMP_COL='date_time',
TIME_SERIES_DATA_COL='WQI_final',
DATA_FREQUENCY='DAILY')
AS
SELECT
DATE(Created_at) as date_time,
AVG(WQI) AS WQI_final
FROM
`Water.water_data`
group by date_time;


-------Evaluating the model
select * from ML.EVALUATE(MODEL Water.forecast_wqi);


-----------Forecasting
select * from ML.FORECAST(MODEL Water.forecast_wqi, STRUCT(7 AS horizon, 0.90 AS confidence_level));


--------Preparing data for visualization
create or replace view `Water.outputdata_datastudio`
AS (
  SELECT
  DATE(Created_at) AS timestamp,
  AVG(WQI) AS history_value,
  NULL AS forecast_value,
  NULL AS prediction_interval_lower_bound,
  NULL AS prediction_interval_upper_bound
  FROM
  `waterqualitymonitoring-385519.Water.water_data`
  group by timestamp
  UNION ALL
  SELECT
  EXTRACT(DATE FROM forecast_timestamp) AS timestamp,
  NULL AS history_value,
  forecast_value,
  prediction_interval_lower_bound,
  prediction_interval_upper_bound
  FROM
  ML.FORECAST(MODEL `Water.forecast_wqi`, 
  STRUCT(7 AS horizon, 0.9 AS confidence_level))
  order by timestamp
);








