<template>
    <div class="node-form">
        <button v-for="(item, idx) in nodeList" :href="item.mac_address" class='node-item'
         @click="$emit('choice', item.mac_address)">
            {{ item.name }} {{ item.mac_address }}
        </button>
    </div>
</template>

<script setup>
import axios from 'axios';
import { onMounted, ref } from 'vue';

let nodeList = ref([]);
const emit = defineEmits(['choice'])

onMounted(() => {
    getNodeList();
});

const getNodeList = () => {
    axios.get('http://pcs.pah.kr:82/hardware').then(res => {
        console.log(res.data);
        nodeList.value = res.data;
    }).catch(err => {
        console.error('노드 목록 조회 실패');
        // default value
        nodeList.value = [
            {
                mac_address: "48:3F:DA:0C:B2:F0",
                name: 'node1'
            },
            {
                mac_address: "14:ED:C9:75:54:DC",
                name: 'node2'
            },
            {
                mac_address: "2C:F4:32:7B:C6:B1",
                name: 'node3'
            },
            {
                mac_address: "EC:FA:BC:86:22:09",
                name: 'node4'
            }
        ];
    })

}

</script>

<style scoped>
.node-form {
    display: flex;
    flex-direction: column;
}

.node-item {
    background-color: white;
    border-radius: 10px;
    min-height: 50px;
    color: black;
    box-sizing: border-box;
    padding: 10px;
    margin: 10px;
}
</style>