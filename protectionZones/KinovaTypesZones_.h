/**
 * @brief This represents a group of limitations that can be applied to a trajectory point.
 */
struct ZoneLimitation
{
	/**
	 * @brief The first speed parameter. Used in angular control, it is the velocity of the actuator 1, 2 and 3 and if used in cartesian
	 * control, it is the translation velocity.
	 */
	float speedParameter1;

	/**
	 * @brief The second speed parameter. Used in angular control, it is the velocity of the actuator 4, 5 and 6 and if used in cartesian
	 * control, it is the orientation velocity.
	 */
	float speedParameter2;

	/**
	 * @brief Not used for now.
	 */
	float speedParameter3;

	/**
	 * @brief Not used for now.
	 */
	float forceParameter1;

	/**
	 * @brief Not used for now.
	 */
	float forceParameter2;

	/**
	 * @brief Not used for now.
	 */
	float forceParameter3;

	/**
	 * @brief Not used for now.
	 */
	float accelerationParameter1;

	/**
	 * @brief Not used for now.
	 */
	float accelerationParameter2;

	/**
	 * @brief Not used for now.
	 */
	float accelerationParameter3;
};

/**
 * @brief This represents the type of a 3d shape.
 */
enum ShapeType
{
	/**
	 * Not used for now.
	 */
	PrismSquareBase_X = 0,

	/**
	 * Not used for now.
	 */
	PrismSquareBase_Y = 1,

	/**
	 * A rectangular prism.
	 */
	PrismSquareBase_Z = 2,

	/**
	 * Not used for now.
	 */
	PrismTriangularBase_X = 3,

	/**
	 * Not used for now.
	 */
	PrismTriangularBase_Y = 4,

	/**
	 * Not used for now.
	 */
	PrismTriangularBase_Z = 5,

	/**
	 * Not used for now.
	 */
    Pyramid = 6
};

/**
 * @brief Represents the 3D shape of a protection zone.
 */
struct ZoneShape
{
	/**
	 * @brief This is the geometric type of shape.
	 */
	ShapeType shapeType;

	/**
	 * @brief Not used for now.
	 */
	int Expansion1;

	/**
	 * @brief The points that describe the shape.
	 */
	CartesianInfo Points[LEGACY_CONFIG_NB_POINTS_COUNT];
};

/**
 * @brief that represents a protection zone.
 */
struct Zone
{
	/**
	 * @brief ID of the zone.
	 */
	int ID;

	/**
	 * @brief Not used for now
	 */
	int Expansion1;

	/**
	 * @brief Geometric shape of the protection zone.
	 */
	ZoneShape zoneShape;

	/**
	 * @brief Limitation to apply inside the protection zone. As an example, if you want the zone to be am unaccessible zone,
	 * you need to limit the velocity to 0.
	 */
	ZoneLimitation zoneLimitation;

	/**
	 * @brief Not used for now.
	 */
	int Expansion2;
};

/**
 * @brief This structure represents the complete list of protection zone of the robotical arm.
 */
struct ZoneList
{
	/**
	 * @brief This is the active zone count.
	 */
	int NbZones;

	/**
	 * @brief Not used for now.
	 */
	int Expansion1;

	/**
	 * @brief This is the list of zone itself.
	 */
	Zone Zones[LEGACY_CONFIG_NB_ZONES_MAX];

};


