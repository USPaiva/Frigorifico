# influxdb_manager.py

from influxdb_client import InfluxDBClient
import pandas as pd
from Frigorifico.Config.Config import influxdb_url,influxdb_token,influxdb_org,influxdb_bucket

class InfluxDBManager:
    def __init__(self):
        self.client = InfluxDBClient(url=influxdb_url, token=influxdb_token, org=influxdb_org)
        self.query_api = self.client.query_api()

    def fetch_data(self, measurement, start_time="-1h"):
        query = f'''
            from(bucket: "{influxdb_bucket}")
            |> range(start: {start_time})
            |> filter(fn: (r) => r["_measurement"] == "{measurement}")
        '''
        return self.query_api.query(org=influxdb_org, query=query)

    def parse_data(self, result):
        data = {
            'Umid': [],
            'Temp': [],
            'Vent1': [],
            'Vent2': []
        }

        for table in result:
            for record in table.records:
                field = record.get_field()
                if field in data:
                    data[field].append(record.get_value())
        
        return data

    def save_to_excel(self, data, filename):
        df = pd.DataFrame(data)
        df.to_excel(filename, index=False)
