<template>
  <BaseModal :name="$options.name">
    <!-- Title slot -->
    <template #title="{ attrs }">
      <div
        v-bind="attrs"
        class="mb-8"
      >
        <fa-icon icon="far fa-circle-question" /> Console help
      </div>
    </template>
    <!-- Default slot: content -->
    <template #default="{ attrs }">
      <div v-bind="attrs">
        <div>
          <p>
            Type any command you want in the prompt input.
          </p>
          <p>
            You can use the <b>ArrowUp</b> (<fa-icon icon="fas fa-arrow-up" />) and the <b>ArrowDown</b> (<fa-icon icon="fas fa-arrow-down" />) keys to navigate through your commands history.
          </p>
          <p>
            To save the console output as plain text (<code>.txt</code>), click on the <fa-icon icon="fas fa-download" /><b>Save</b> button.
          </p>
          <p>
            To clear the console output, click on the <fa-icon icon="fas fa-trash" /><b>Clear</b> button.
          </p>
        </div>
      </div>
    </template>
  </BaseModal>
</template>

<script setup lang="ts">
import { BaseModalComponent as BaseModal } from '@/lib/modals'

defineOptions({
  name: 'ConsoleHelpModal'
})
</script>
