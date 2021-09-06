/*
 * Copyright (c) 2020 Sebastien Marie <semarie@online.fr>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/stdarg.h>
#include <ddb/db_output.h>

#include <dev/pci/drm/amd/amdgpu/amdgpu.h>
#include <dev/pci/drm/amd/amdgpu/amdgpu_ring.h>

/* small type helper for readability */
typedef int (*db_printf_t)(const char *, ...)
	__attribute__((__format__(__kprintf__,1,2)));


void db_amdgpu_devices_print(db_printf_t);
void db_amdgpu_device_print(struct amdgpu_device *, db_printf_t);
void db_amdgpu_ring_print(struct amdgpu_ring *, db_printf_t);
void db_gpu_scheduler_print(struct drm_gpu_scheduler *, db_printf_t);
void db_gpu_scheduler_runqueue_print(struct drm_sched_rq *, db_printf_t);


#if 1 /* helper to use, from ddb(4): call db_xxx() */
void
db_xxx(void)
{
	KERNEL_LOCK();
	db_amdgpu_devices_print(db_printf);
	KERNEL_UNLOCK();
}
#endif


void
db_amdgpu_devices_print(db_printf_t pr)
{
	int i;

	for (i = 0; i < mgpu_info.num_gpu; i++) {
		db_amdgpu_device_print(mgpu_info.gpu_ins[i].adev, pr);
		pr("\n");
	}
}

void
db_amdgpu_device_print(struct amdgpu_device *adev, db_printf_t pr)
{
	int i;

	if (adev == NULL)
		return;
	
	pr("amdgpu_device %p\n", adev);
	pr("  %d rings\n", adev->num_rings);
	for (i = 0; i < adev->num_rings; i++)
		db_amdgpu_ring_print(adev->rings[i], pr);
}

void
db_amdgpu_ring_print(struct amdgpu_ring *ring, db_printf_t pr)
{
	if (ring == NULL || ring->sched.name == NULL)
		return;
	
	pr("ring %p '%s' (adev %p)\n", ring, ring->sched.name, ring->adev);
	db_gpu_scheduler_print(&ring->sched, pr);
#if 0
	if (ring->fence_drv.initialized)
		db_dma_fences_print(&ring->fence_drv.fences);
#endif
}

void
db_gpu_scheduler_print(struct drm_gpu_scheduler *sched, db_printf_t pr)
{
	extern struct kthread *kthread_lookup(struct proc *);
	int i;

	if (sched == NULL || sched->name == NULL)
		return;
	
	pr("sched %p (ring '%s')\n", sched, sched->name);
	if (sched->thread) {
		pr("  thread (struct proc)    %p (%s) %s\n", sched->thread,
		    sched->thread->p_p->ps_comm,
		    sched->thread->p_wchan ? sched->thread->p_wmesg : "-");
		pr("  thread (struct kthread) %p\n", kthread_lookup(sched->thread));
	} else {
		pr("  thread (struct proc)    0x0\n");
		pr("  thread (struct kthread) 0x0\n");
	}
	pr("  hw_submission_limit %d\n", sched->hw_submission_limit);
	pr("  hw_rq_count %d\n", sched->hw_rq_count);

	/* struct list_head                ring_mirror_list */

	/* runqueue ordered by priority */
	for (i = DRM_SCHED_PRIORITY_COUNT - 1; i >= DRM_SCHED_PRIORITY_MIN; i--)
		db_gpu_scheduler_runqueue_print(&sched->sched_rq[i], pr);
}

void
db_gpu_scheduler_runqueue_print(struct drm_sched_rq *rq, db_printf_t pr)
{
	struct drm_sched_entity *entity;

	if (rq == NULL)
		return;
	
	spin_lock(&rq->lock);

	list_for_each_entry(entity, &rq->entities, list) {
		struct dma_fence *fence;
		
		pr("entity %p%s %s%s%s\n", entity,
		    rq->current_entity == entity ? " (current)" : "",
		    drm_sched_entity_is_ready(entity) ? "R" : "r",
		    (list_empty(&entity->list) || spsc_queue_count(&entity->job_queue) == 0) ? "I" : "i",
		    entity->guilty ? "G" : "g",
		    entity->stopped ? "S" : "s");
		pr("  rq %p\n", entity->rq);
		pr("  completion %p (%d)\n", &entity->entity_idle, entity->entity_idle.done);
		pr("  job_queue %d\n", spsc_queue_count(&entity->job_queue));
		pr("  priority %d\n", entity->priority);
		pr("  num_sched_list %d\n", entity->num_sched_list);
		pr("  fence_seq %d\n", entity->fence_seq);
		pr("  last_user %p\n", entity->last_user);

		fence = READ_ONCE(entity->last_scheduled);
		if (fence)
			pr("  last_scheduled fence %p %s\n",
			    fence, dma_fence_is_signaled(fence) ? "S" : "s");
		else
			pr("  last_scheduled fence NONE\n");
		pr("\n");
	}
	
	spin_unlock(&rq->lock);
}
