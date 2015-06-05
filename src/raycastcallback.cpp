#include <raycastcallback.hpp>

RaycastCallback::RaycastCallback(const b2Body* owner)
    : owner(owner)
    , fixture(nullptr)
    , point()
    , normal()
    , fraction(0.0f)
    , oldFixture(nullptr)
    , oldFraction(0.0f)
{

}

RaycastCallback::~RaycastCallback()
{

}

float32 RaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
{
    //ignore self
    for (const b2Fixture* f = owner->GetFixtureList(); f; f = f->GetNext())
    {
        if (f==fixture)
        {
            fixture = oldFixture;
            fraction = oldFraction;
            return 1;
        }
    }

    //ignore fixtures joined to self
    for (const b2JointEdge* j = owner->GetJointList(); j; j = j->next)
    {
        for (const b2Fixture*f = j->joint->GetBodyB()->GetFixtureList(); f; f= f->GetNext())
        {
            if (f == fixture)
            {
                fixture = oldFixture;
                fraction = oldFraction;
                return 1;
            }
        }
    }

    this->fixture = fixture;
    this->point = point;
    this->normal = normal;
    this->fraction =  fraction;
    oldFraction = fraction;
    oldFixture = fixture;

    return fraction;
}
