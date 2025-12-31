<template>
  <div>
    <!-- Modal overlay -->
    <transition-group name="modals">
      <component
        :is="modal.component()"
        v-for="(modal, index) in modals"
        :key="index"
        v-bind="modal.props"
        :inert="index < (modals.length - 1)"
      />
    </transition-group>

    <div
      class="relative z-0 flex min-h-dvh flex-col items-center"
      :inert="!!modals.length"
    >
      <Header />

      <main class="flex max-w-5xl flex-1 justify-center p-4">
        <slot />
      </main>

      <Footer />
    </div>
  </div>
</template>

<script setup lang="ts">
import { useModals } from '@/lib/modals'
import Header from './partials/Header.vue'
import Footer from './partials/Footer.vue'

const { modals } = useModals()
</script>

<style>
.modals-enter-active,
.modals-leave-active {
  transition: all 0.2s ease;
}

.modals-enter-from,
.modals-leave-to {
  opacity: 0;
}
</style>
