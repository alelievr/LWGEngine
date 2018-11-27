#pragma once

#include <iostream>
#include <string>

#include "IncludeDeps.hpp"

#include GLFW_INCLUDE
#include "ShaderSource.hpp"

namespace LWGC
{
	class		ShaderProgram
	{
		private:
			ShaderSource	_fragmentShaderSource;
			ShaderSource	_vertexShaderSource;
			ShaderSource	_geometryShaderSource;
			ShaderSource	_computeShaderSource;

			std::vector< ShaderSource * >					_shaderSources;
			std::vector< VkPipelineShaderStageCreateInfo >	_shaderStages;

		public:
			ShaderProgram(void);
			ShaderProgram(const ShaderProgram &) = delete;
			virtual ~ShaderProgram(void);

			ShaderProgram &	operator=(ShaderProgram const & src) = delete;

			void	CreateStages(void);

			void	SetVertexSourceFile(const std::string & file);
			void	SetFragmentSourceFile(const std::string & file);
			void	SetGeometrySourceFile(const std::string & file);
			void	SetComputeSourceFile(const std::string & file);

			void	Bind(void);
			bool	Update(void);

			VkPipelineShaderStageCreateInfo *	GetShaderStages();
	};

	std::ostream &	operator<<(std::ostream & o, ShaderProgram const & r);
}
