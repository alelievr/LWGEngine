#pragma once

#include <iostream>
#include <string>

#include "Core/Object.hpp"
#include "Core/Mesh.hpp"
#include "Core/Vulkan/Material.hpp"
#include "Core/Vulkan/UniformBuffer.hpp"
#include "Component.hpp"
#include "Core/Components/Renderer.hpp"

namespace LWGC
{
	class		ProceduralRenderer : public Renderer
	{
		private:
			int			_verticeCount;
			int			_elementCount;

			void		RecordDrawCommand(VkCommandBuffer cmd) noexcept override;

		public:
			ProceduralRenderer(void);
			ProceduralRenderer(const ProceduralRenderer &) = delete;
			ProceduralRenderer(const PrimitiveType prim, std::shared_ptr< Material > material);
			// TODO: the primitiveType constructor must be into Mesh.cpp
			// TODO: Add a constructor with a mesh and a material
			ProceduralRenderer(const PrimitiveType primitiveType);
			virtual ~ProceduralRenderer(void);

			ProceduralRenderer &	operator=(ProceduralRenderer const & src) = delete;

			void	SetModel(const Mesh & mesh, const Material & material);
			void	SetModel(std::shared_ptr< Mesh > mesh, std::shared_ptr< Material > material);

			int		GetVerticeCount(void) const noexcept;
			void	SetVerticeCount(int verticeCount) noexcept;

			int		GetElementCount(void) const noexcept;
			void	SetElementCount(int verticeCount) noexcept;

			virtual uint32_t	GetType(void) const noexcept override;
	};

	std::ostream &	operator<<(std::ostream & o, ProceduralRenderer const & r);
}
