#pragma once

#include <unordered_set>

#include "Core/Transform.hpp"
#include "IncludeDeps.hpp"
#include "Core/Delegate.tpp"
#include VULKAN_INCLUDE

namespace LWGC
{
	class GameObject;
	class Hierarchy;
	class Component;

    using ComponentIndex = std::unordered_set< Component * >::iterator;

	enum class	ComponentType : uint32_t
	{
		MeshRenderer,
		ProceduralRenderer,
		Light,
		Camera,
		FreeCameraControls,
		ComputeDispatcher,
		Rotator,
		Movator,
		Activator,
		ImGUIPanel,
		IndirectRenderer,

 		// Note: this MUST be the last element of the enum
		Count,
	};

	class Component
	{
		friend class GameObject;

		private:
			bool					oldState;

		protected:
			bool					enabled;
			GameObject *			gameObject;
			Transform *				transform;
			Hierarchy *				hierarchy;
			ComponentIndex			index;
			VkDevice				device;
			DelegateIndex< void() >	updateIndex;

			// Called when the vulkan is finished to initialize
			virtual void		Initialize() noexcept;
			void				UpdateGameObjectActive() noexcept;

		public:
			Component(void);
			Component(const Component & comp) = delete;
			virtual ~Component(void);

			Component operator=(const Component & rhs) = delete;

			virtual void	OnAdded(GameObject & go) noexcept;
			virtual void	OnRemoved(const GameObject & go) noexcept;
			virtual void	OnEnable() noexcept;
			virtual void	OnDisable() noexcept;
			virtual	void	Update() noexcept;
			bool			IsEnabled() noexcept;

			void			Enable() noexcept;
			void			Disable() noexcept;

			Transform *		GetTransform() const noexcept;
			GameObject *	GetGameObject() const noexcept;

			virtual uint32_t	GetType(void) const noexcept;
	};
}
