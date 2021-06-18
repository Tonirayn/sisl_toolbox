#pragma once

#include <iostream>
#include <memory>
#include <fstream>
#include <vector>

#include <eigen3/Eigen/Dense>

struct SISLCurve; /** Forward declaration */
class CurveFactory; /** Forward declaration */

/**
 * @class Curve
 *
 * @brief The main objective of this class is to define a wrapper for the most used SISL functions and to provide an in meters curve parametrization,
 *        internally applying a conversion from meters to Sisl parametrization. 
 */
class Curve {

public:
    /** 
     * @brief Curve constructor.
     * 
     * @param dimension Define dimension of curve
     * @param order Define order of curve
     */ 
    Curve(int dimension = 3, int order = 3);

    /** 
     * @brief Curve constructor.
     * 
     * @param curve Pointer to the curve 
     * @param  dimension Define dimension of curve
     * @param order Define order of curve
     * 
     */ 
    Curve(SISLCurve * curve, int dimension = 3, int order = 3);

    /**
    * @brief Convert from Sisl parametrization to Meters parametrization. If the input abscissa is out of range, an exception is thrown. 
    * @param[in] abscissa_s Starting position (abscissa in Sisl parametrization) of the point.
    * 
    * @return A tuple containing respectively: the abscissa (in meters).
    */
    double SislAbsToMeterAbs(double abscissa_s);

    /**
    * @brief Convert from Meters parametrization to Sisl parametrization. If the input abscissa is out of range, an exception is thrown.
    * @param[in] abscissa_m Starting position (abscissa in meters parametrization) of the point.
    * 
    * @return A tuple containing respectively: the abscissa (in Sisl parametrization).
    */
    double MeterAbsToSislAbs(double abscissa_m); 

    /**
    * @brief Convert an abscissa value (in Sisl) to a position in world frame.
    * @param[in] abscissa_s Abscissa to compute the position.
    * @param[out] worldF_position Eigen::Vector3d& containing the position.
    */
    void FromAbsSislToPos(double abscissa_s, Eigen::Vector3d& worldF_position);

    /**
    * @brief Convert an abscissa value (in meters) to a position in world frame.
    * @param[in] abscissa_m Abscissa to compute the position.
    * @param[out] worldF_position Eigen::Vector3d& containing the position.
    */
    void FromAbsMetersToPos(double abscissa_m, Eigen::Vector3d& worldF_position);

    /**
     * @brief Given an abscissa in meters return the corresponding point on the curve.
     * 
     * @param[in] abscissa_m abscissa on the curve in meters.
     *  
     * @return Eigen::Vector3d containing the point at abscissa_m.
     */
    Eigen::Vector3d At(double abscissa_m);

    /**
     * @brief Given an abscissa in meters return the derivatives up to the n-th one at abscissa_m point.
     * 
     * @param[in] order evaluate the derivatives from order 1 up to order.
     * @param[in] abscissa_m abscissa on the curve in meters.
     *  
     * @return Eigen::Vector3d containing the point at abscissa_m.
     */
    std::vector<Eigen::Vector3d> Derivate(int order, double abscissa_m);

    /**
     * @brief Evaluate the curvature of the curve at a given parameter value.
     * 
     * @param[in] abscissa_m abscissa on the curve in meters
     * 
     * @return curvature value
     */ 
    double Curvature(double abscissa_m);
    
    /**
    * @brief Turns the direction of the orginal curve.
    */
    void Reverse();

    /**
    * @brief Samples the curve. 
    * @param[in] samples Samplesto be produces.
    * 
    * @return A <std::vector<Eigen::Vector3d>> containing the points.
    */
    std::shared_ptr<std::vector<Eigen::Vector3d>> Sampling(int const samples) const;

    /**
    * @brief Find the closest point between a curve and a point.
    * @details Find the closest point between a curve and a point. The method is fast and should work well in clear cut cases but does not guarantee 
    * finding the right solution. As long as it doesn’t fail, it will find exactly one point. In other cases, use s1953().
    * @param[in] position The point in the closest point problem.
    * 
    * @return A tuple (double, double) containing as first element the abscissa_m (in meters) of the on curve point solution of 
    * the closest point problem. * The second element is the distance between the point passed as argument (worldF_position) 
    * and the point on curve solution of the closest point problem.
    */
    std::tuple<double, double> FindClosestPoint(Eigen::Vector3d& worldF_position);

    /**
    * @brief Pick a part of a curve.
    * @details It extracts a new curve from the stating one according to the abscissa startValue and endValue.
    * @param[in] startValue_m Start parameter value of the part curve to be picked (in meters).
    * @param[in] endValue_m End parameter value of the part curve to be picked (in meters).
    * 
    * @return A shared ptr to the curve object.
    */
    std::shared_ptr<Curve> ExtractSection(double startValue_m, double endValue_m);

    /**
    * @brief Eval intersection points between two curves.
    * 
    * @param[in] otherCurve The other curve w.r.t. evaluate the intersections.
    * 
    * @return An std::vector<Eigen::Vector3d> containing all the intersection points.
    */
    std::vector<Eigen::Vector3d> Intersection(std::shared_ptr<Curve> otherCurve);




    /** TODO: Aggiustare!!! */
    /**
    * @brief Transform the abscissa value into a distance in meters from the starting point.
    * @details The tangntial direction depends on the direction of the curve. Starting from this vector, the normal component is calculated as cross product among 
    * the tangent vector and the z versor of the world frame. The binormal direction is compute as cross product among tangent and normal direction.
    * @param[in] abscissa Parameter value where to calculate the tangent frame.
    * @param[out] tangent Tangent component of the tangent 3D frame.
    * @param[out] normal Normal component of the tangent 3D frame.
    * @param[out] binormal Binormal component of the tangent 3D frame.
    */
    void EvalTangentFrame(double abscissa_m, Eigen::Vector3d& tangent, Eigen::Vector3d& normal, Eigen::Vector3d& binormal);


    



    friend std::ostream& operator<< (std::ostream& os, const Curve& obj) {
        return os 
            << "Curve name: " << obj.name_
            << " | Length: " << (obj.length_)
            << " | In meters parametrization interval: [" << obj.startParameter_m_ << ", " << obj.endParameter_m_ << "]"
            << " | Sisl parametrization interval: [" << obj.startParameter_s_ << ", " << obj.endParameter_s_ << "]";
    };


    // Getters
    auto Dimension() const& {return dimension_;}
    auto Order() const& {return order_;}
    auto Epsge() const& {return epsge_;}
    auto CurvePtr() const& {return curve_;}
    auto StatusFlag() const& {return statusFlag_;}
    auto StartParameter_s() const& {return startParameter_s_;}
    auto EndParameter_s() const& {return endParameter_s_;}
    auto StartParameter_m() const& {return startParameter_m_;}
    auto EndParameter_m() const& {return endParameter_m_;}
    auto Length() const& {return length_;}
    auto StartPoint() const& {return startPoint_;}
    auto EndPoint() const& {return endPoint_;}
    auto Name() const& {return name_;}


private:

    friend class CurveFactory;

    int dimension_; // Dimension of the curve
    int order_; // Order of the curve
    double epsge_; // Geometric resolution

protected:
    SISLCurve *curve_;
    int statusFlag_; // Control flag used as output of each SISL function

    std::string name_;
    double length_;
    double startParameter_s_; // Start value of the curve parametrization
    double endParameter_s_; // Last value of the curve parametrization  
    double startParameter_m_;
    double endParameter_m_;  
    Eigen::Vector3d startPoint_; // Curve start point
    Eigen::Vector3d endPoint_; // Curve end point

};

/*** NOTE: mkdir build -> cmake .. -DBUILD_TESTS=ON -> sudo make install ***/
