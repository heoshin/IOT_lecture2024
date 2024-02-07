<script setup>
import HelloWorld from './components/HelloWorld.vue'
import NodeList from './pages/NodeList.vue'
import NodeDetail from './pages/NodeDetail.vue';
import { ref } from 'vue'

const choiceNodeId = ref(null);

</script>

<template>
  <div v-if="choiceNodeId !== null">
    <button @click="choiceNodeId = null">x</button>
    <NodeDetail :nodeId="choiceNodeId" />
  </div>
  <NodeList v-else @choice="nodeId => choiceNodeId = nodeId" />
</template>

<style scoped></style>
