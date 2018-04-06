/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Transform.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/08/31 11:42:19 by alelievr          #+#    #+#             */
/*   Updated: 2018/04/06 21:24:50 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <string>
#include <list>

class		Transform
{
	private:
		Transform *	_parent;
		std::list< Transform >	_childs;
		glm::vec3	_position;
		glm::quat	_rotation;
		glm::vec3	_scale;
		glm::vec3	_up;
		glm::vec3	_down;
		glm::vec3	_right;
		glm::vec3	_left;
		glm::vec3	_forward;
		glm::vec3	_back;
		glm::vec3	_eulerAngles;


	public:
		Transform(void);
		Transform(const Transform&);
		virtual ~Transform(void);

		Transform &	operator=(Transform const & src);

		void	Rotate(const glm::vec3 & eulerAngles);

		void	Rotate(const float xAngle, const float yAngle, const float zAngle);

		void	RotateAround(const glm::vec3 & point, const glm::vec3 & axis, const float angle);

		size_t	GetChildCount(void);

		bool	IsChildOf(const Transform & t);

		Transform	GetChildAt(const int index) const;

		void	LookAt(const int index);

		glm::vec3	TransformDirection(const glm::vec3 & direction);

		glm::vec3	TransformDirection(const float x, const float y, const float z);

		glm::vec3	TransformPoint(const glm::vec3 & position);

		glm::vec3	TransformPoint(const float x, const float y, const float z);

		void	Translate(const glm::vec3 & translation);

		Transform &	GetRoot(void);

		Transform *	GetParent(void) const;
		void	SetParent(Transform * tmp);
		
		std::list< Transform >	GetChilds(void) const;
		void	SetChilds(std::list< Transform > tmp);
		
		glm::vec3	GetPosition(void) const;
		void	SetPosition(glm::vec3 tmp);
		
		glm::quat	GetRotation(void) const;
		void	SetRotation(glm::quat tmp);
		
		glm::vec3	GetScale(void) const;
		void	SetScale(glm::vec3 tmp);
		
		glm::vec3	GetUp(void) const;
		void	SetUp(glm::vec3 tmp);
		
		glm::vec3	GetDown(void) const;
		void	SetDown(glm::vec3 tmp);
		
		glm::vec3	GetRight(void) const;
		void	SetRight(glm::vec3 tmp);
		
		glm::vec3	GetLeft(void) const;
		void	SetLeft(glm::vec3 tmp);
		
		glm::vec3	GetForward(void) const;
		void	SetForward(glm::vec3 tmp);
		
		glm::vec3	GetBack(void) const;
		void	SetBack(glm::vec3 tmp);
		
		glm::vec3	GetEulerAngles(void) const;
		void	SetEulerAngles(glm::vec3 tmp);
};

std::ostream &	operator<<(std::ostream & o, Transform const & r);
