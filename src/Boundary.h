/*
  Copyright (c) 2015-2017 Lester Hedges <lester.hedges+slsm@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _BOUNDARY_H
#define _BOUNDARY_H

#include <vector>

#include "Common.h"

/*! \file Boundary.h
    \brief A class for the discretised boundary.
 */

namespace slsm
{
    // FORWARD DECLARATIONS

    class LevelSet;
    class Mesh;

    // ASSOCIATED DATA TYPES

    //! \brief A container for storing information associated with a boundary point.
    class BoundaryPoint
    {
    public:
        //! Constructor.
        BoundaryPoint();

        Coord coord;                            //!< Coordinate of the boundary point.
        Coord normal;                           //!< Inward pointing normal vector.
        double length;                          //!< Integral length of the boundary point.
        double velocity;                        //!< Normal velocity (positive acts inwards).
        double negativeLimit;                   //!< Movement limit in negative direction (inwards).
        double positiveLimit;                   //!< Movement limit in positive direction (outwards).
        bool isDomain;                          //!< Whether the point lies close to the domain boundary.
        bool isFixed;                           //!< Whether the point is fixed.
        unsigned int nSegments;                 //!< The number of boundary segments that a point belongs to.
        std::vector<unsigned int> segments;     //!< The indices of the two segments to which a point belongs.
        unsigned int nNeighbours;               //!< The number of neighbouring boundary points.
        std::vector<unsigned int> neighbours;   //!< The indices of the neighbouring points.
        std::vector<double> sensitivities;      //!< Objective and constraint sensitivities.
    };

    //! \brief A container for storing information associated with a boundary segment.
    class BoundarySegment
    {
    public:
        //! Constructor.
        BoundarySegment();

        unsigned int start;                     //!< Index of start point.
        unsigned int end;                       //!< Index of end point.
        unsigned int element;                   //!< The element cut by the boundary segment.
        double length;                          //!< Length of the boundary segment.
        double weight;                          //!< Weighting factor for boundary segment.
    };

    // MAIN CLASS

    /*! \brief A class for computing the discretised boundary of the level set zero contour.

        The boundary is a set of points that represent a piece-wise linear
        discretisation of the zero contour of the level set. Boundary points
        either lie exactly on a node of the level set (when it has a zero value)
        or along an edge if the level set changes sign between two adjacent nodes.
        The position of these boundary points is found using linear interpolation.

        The points vector holds coordinates for boundary points. Boundary segment
        data is stored in the segments vector. Each segment is constructed from
        two adjacent boundary points.

        Class member functions allow the user to compute the current discretisation
        of the zero contour, to evaluate the perimeter of the boundary, to calculate
        the amount of material area in each of the cells of the level set domain,
        and to determine the inward pointing normal vector at each boundary point.
     */
    class Boundary
    {
    public:
        //! Constructor.
        Boundary();

        //! Use linear interpolation to compute the discretised boundary
        /*! \param levelSet
                A reference to the level set object.

            \param isTarget
                Whether to discretise the target signed distance function.
         */
        void discretise(LevelSet&, bool isTarget = false);

        //! Compute the local normal vector at each boundary point.
        /*! \param levelSet
                A reference to the level set object.
         */
        void computeNormalVectors(const LevelSet&);

        //! Compute the local perimeter for a boundary point.
        /*! \param point
                A reference to the boundary point.

            \return
                The perimeter around the boundary point.
         */
        double computePerimeter(const BoundaryPoint&);

        /// Vector of boundary points.
        std::vector<BoundaryPoint> points;

        /// Vector of boundary segments.
        std::vector<BoundarySegment> segments;

        /// The number of boundary points.
        unsigned int nPoints;

        /// The number of boundary segments.
        unsigned int nSegments;

        /// The total length of the boundary.
        double length;

    private:
        //! Determine the status of the elements and nodes of the level set mesh.
        /*! \param mesh
                A reference to the fixed-grid mesh.

            \param signedDistance
                A pointer to the signed distance function vector.
         */
        void computeMeshStatus(Mesh&, const std::vector<double>* signedDistance) const;

        //! Check whether a boundary point has already been added.
        /*! \param mesh
                A reference to the fixed-grid mesh.

            \param point
                The coordinates of the boundary point (to be determined).

            \param node
                The index of the adjacent node.

            \param edge
                The index of the element edge.

            \param distance
                The distance from the node.

            \return
                The index of the boundary point if previously added, minus one if not.
         */
        int isAdded(Mesh&, Coord&, const unsigned int&, const unsigned int&, const double&);

        //! Initialise a boundary point.
        /*! \param levelSet
                A reference to the level set object.

            \param point
                A reference to a boundary point.

            \param coord
                The position vector of the boundary point.
         */
        void initialisePoint(LevelSet&, BoundaryPoint&, const Coord&);

        //! Return the length of a boundary segment.
        /*! \param segment
                A reference to the boundary segment.

            \return
                The length of the boundary segment.
         */
        double segmentLength(const BoundarySegment&);

        //! Compute the (potentially weighted) integral length for each boundary point.
        void computePointLengths();
    };
}

#endif  /* _BOUNDARY_H */
