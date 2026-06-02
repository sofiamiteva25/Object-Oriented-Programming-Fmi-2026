//#pragma once

#include <string>
#include <iostream>
#include <optional>
using namespace std;
class URL {

public:

    struct Builder {
        std::string protocol;
        std::string subdomain;
        std::string domain;
        //optional
        std::string path;
        std::string country;
        optional<int> discount;

        Builder(const string& _protocol, const string& _subdomain, const string& _domain)
        : protocol(_protocol), subdomain(_subdomain), domain(_domain){
            
        }

        Builder& setPath(const string& path) {
            this->path = path;
            return *this;
        }
        Builder& setCountry(const string& _country) {
            country = _country;
            return *this;
        }
        Builder& setDiscount(int d) {
            discount = d;
            return *this;
        }

        URL build()  const {
            return URL(*this);
        }
    };

    static Builder builder(const string& _protocol, const string& _subdomain, const string& _domain) {
        return Builder(_protocol, _subdomain, _domain);
    }
    // Convert to string
    std::string toString() const {
        std::string url = protocol + "://" + subdomain + "." + domain;
        if (!path.empty()) url += path;
        if (!country.empty()) {
            url += "?country=";
            url += country;
        }
        if (discount.has_value()) {
            url += "&discount=";
            url += to_string(discount.value()) + '%';
        }
        return url;
    }
    URL(const string& _protocol, const string& _subdomain, const string& _domain,
        const string& _path = "", const string& _country = "", const optional<int>& discount=nullopt) :
        protocol(_protocol),
        subdomain(_subdomain),
        domain(_domain),
        path(_path),
        country(_country),
        discount(discount)
    {

    }

private:
    std::string protocol;
    std::string subdomain;
    std::string domain;
    //optional
    std::string path;
    std::string country;
    optional<int> discount;

    explicit URL(const Builder& b) :
        protocol(b.protocol),
        subdomain(b.subdomain),
        domain(b.domain),
        path(b.path),
        country(b.country),
        discount(b.discount)
    {

    }
};

int main() {
     URL url = URL::builder("https", "www", "example.com")
         .setPath("/index.html")
         .setCountry("BG")
         .setDiscount(50)
                    .build();
   // URL url("https", "www", "example.com", "/index.html","BG", std::nullopt);
    std::cout << url.toString() << std::endl;
    return 0;
}
