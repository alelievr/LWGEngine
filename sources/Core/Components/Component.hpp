#pragma once

#include <unordered_set>

namespace LWGC
{
	class GameObject;
	class Hierarchy;

	class Component;
	
    using ComponentIndex = std::unordered_set< Component * >::iterator;

	class Component
	{
		friend class GameObject;
	
		private:
			bool				oldState;
		
		protected:
			bool				enabled;
			const GameObject *	gameObject;
			Hierarchy *			hierarchy;
			ComponentIndex		renderContextIndex;

			// Called when the vulkan is finished to initialize
			virtual void		Initialize() noexcept;
			void				UpdateGameObjectActive() noexcept;

		public:
			Component(void);
			Component(const Component & comp) = delete;
			virtual ~Component(void);

			Component operator=(const Component & rhs) = delete;

			virtual void OnAdded(const GameObject & go) noexcept;
			virtual void OnRemoved(const GameObject & go) noexcept;
			virtual void OnEnable() noexcept;
			virtual void OnDisable() noexcept;
			bool IsEnabled() noexcept;

			void			Enable() noexcept;
			void			Disable() noexcept;
	};
}
