<template>
    <VueApexCharts width="100%" type="line" :options="chartOptionsTemperature" :series="chartSeriesTemperature"></VueApexCharts>
    <VueApexCharts width="100%" type="line" :options="chartOptionsHumidity" :series="chartSeriesHumidity"></VueApexCharts>
    <VueApexCharts width="100%" type="line" :options="chartOptionsBrightness" :series="chartSeriesBrightness"></VueApexCharts>
</template>
<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import VueApexCharts from 'vue3-apexcharts'

const props = defineProps({
    nodeId: String,
});

onMounted(() => {
    getNodeDetail(props.nodeId);
});

const chartOptionsTemperature = ref({
    chart: {
        id: 'vuechart-temperature'
    }
});
const chartSeriesTemperature = ref([{
    name: 'Temperature',
    data: [],
}]);

const chartOptionsHumidity = ref({
    chart: {
        id: 'vuechart-humidity'
    }
});
const chartSeriesHumidity = ref([{
    name: 'Humidity',
    data: [],
}]);

const chartOptionsBrightness = ref({
    chart: {
        id: 'vuechart-brightness'
    }
});
const chartSeriesBrightness = ref([{
    name: 'Brightness',
    data: [],
}]);

let nodeDetail = [];
const getNodeDetail = (nodeId) => {
    let apiUrl = `http://pcs.pah.kr:82/hardware/${nodeId}?interval=minute`;
    axios.get(apiUrl).then(res => {
        console.log(res.data);
        nodeDetail = res.data;
    }).catch(err => {
        console.error('노드 데이터 조회 실패');
        // default value
        nodeDetail.value = {
            name: 'node-dummy',
            data: [
                {
                    temperature: 22.799999237060547,
                    humidity: 34.70000076293945,
                    brightness: 579,
                    timestamp: "2024-02-05 18:46:18.784217"
                },
                {
                    temperature: 22.799999237060547,
                    humidity: 34.70000076293945,
                    brightness: 634,
                    timestamp: "2024-02-05 18:46:17.784088"
                },
                {
                    temperature: 22.799999237060547,
                    humidity: 34.70000076293945,
                    brightness: 645,
                    timestamp: "2024-02-05 18:46:16.783550"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 34.79999923706055,
                    brightness: 634,
                    timestamp: "2024-02-05 18:46:15.767377"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 34.79999923706055,
                    brightness: 645,
                    timestamp: "2024-02-05 18:46:14.782460"
                },
                {
                    temperature: 22.799999237060547,
                    humidity: 34.900001525878906,
                    brightness: 623,
                    timestamp: "2024-02-05 18:46:13.781848"
                },
                {
                    temperature: 22.799999237060547,
                    humidity: 34.900001525878906,
                    brightness: 634,
                    timestamp: "2024-02-05 18:46:12.781276"
                },
                {
                    temperature: 22.799999237060547,
                    humidity: 35,
                    brightness: 503,
                    timestamp: "2024-02-05 18:46:11.765092"
                },
                {
                    temperature: 22.799999237060547,
                    humidity: 35,
                    brightness: 524,
                    timestamp: "2024-02-05 18:46:10.780212"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.099998474121094,
                    brightness: 601,
                    timestamp: "2024-02-05 18:46:09.780097"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.099998474121094,
                    brightness: 645,
                    timestamp: "2024-02-05 18:46:08.778860"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.20000076293945,
                    brightness: 513,
                    timestamp: "2024-02-05 18:46:07.777179"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.20000076293945,
                    brightness: 535,
                    timestamp: "2024-02-05 18:46:06.776680"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.20000076293945,
                    brightness: 535,
                    timestamp: "2024-02-05 18:46:05.781081"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.20000076293945,
                    brightness: 535,
                    timestamp: "2024-02-05 18:46:04.786127"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.29999923706055,
                    brightness: 513,
                    timestamp: "2024-02-05 18:46:03.769996"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.29999923706055,
                    brightness: 535,
                    timestamp: "2024-02-05 18:46:02.784976"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.400001525878906,
                    brightness: 524,
                    timestamp: "2024-02-05 18:46:01.784998"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.400001525878906,
                    brightness: 557,
                    timestamp: "2024-02-05 18:46:00.784503"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.400001525878906,
                    brightness: 590,
                    timestamp: "2024-02-05 18:45:59.768322"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.400001525878906,
                    brightness: 623,
                    timestamp: "2024-02-05 18:45:58.783386"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.099998474121094,
                    brightness: 590,
                    timestamp: "2024-02-05 18:45:57.782916"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.099998474121094,
                    brightness: 601,
                    timestamp: "2024-02-05 18:45:56.782342"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35,
                    brightness: 579,
                    timestamp: "2024-02-05 18:45:55.781775"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35,
                    brightness: 590,
                    timestamp: "2024-02-05 18:45:54.781258"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35,
                    brightness: 568,
                    timestamp: "2024-02-05 18:45:53.780680"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35,
                    brightness: 590,
                    timestamp: "2024-02-05 18:45:52.780145"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35,
                    brightness: 568,
                    timestamp: "2024-02-05 18:45:51.779576"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35,
                    brightness: 590,
                    timestamp: "2024-02-05 18:45:50.778929"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.099998474121094,
                    brightness: 535,
                    timestamp: "2024-02-05 18:45:49.778386"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.099998474121094,
                    brightness: 590,
                    timestamp: "2024-02-05 18:45:48.777888"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.20000076293945,
                    brightness: 568,
                    timestamp: "2024-02-05 18:45:47.777358"
                },
                {
                    temperature: 22.899999618530273,
                    humidity: 35.20000076293945,
                    brightness: 579,
                    timestamp: "2024-02-05 18:45:46.777267"
                }
            ]
        };
    }).finally(() => {
        chartSeriesTemperature.value[0].data = nodeDetail.data.map((item, idx) => [new Date(item.timestamp).getTime(), item.temperature])
        chartSeriesHumidity.value[0].data = nodeDetail.data.map((item, idx) => [new Date(item.timestamp).getTime(), item.humidity])
        chartSeriesBrightness.value[0].data = nodeDetail.data.map((item, idx) => [new Date(item.timestamp).getTime(), item.brightness])
    });

}
</script>
<style scoped>
</style>