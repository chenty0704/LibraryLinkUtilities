#pragma once

/// \brief Represents a constant view of a regular time series.
/// \tparam T The type of values.
template<typename T>
struct TimeSeriesView {
    double IntervalSeconds; ///< The interval between two values in seconds.
    span<const T> Values; ///< A list of values.

    /// \brief Returns the length of the path.
    /// \return The length of the path.
    [[nodiscard]] int PathLength() const noexcept {
        return Values.size();
    }

    /// \brief Returns the duration of the path in seconds.
    /// \return The duration of the path in seconds.
    [[nodiscard]] double DurationSeconds() const noexcept {
        return PathLength() * IntervalSeconds;
    }
};

/// \brief Represents a constant view of a collection of regular time series.
/// \tparam T The type of values.
template<typename T>
struct TemporalDataView {
    double IntervalSeconds; ///< The interval between two values in seconds.
    mdspan<const T, dextents<int, 2>> Values; ///< A 2D array of values.

    /// \brief Returns the number of paths in the collection.
    /// \return The number of paths in the collection.
    [[nodiscard]] int PathCount() const noexcept {
        return Values.extent(0);
    }

    /// \brief Returns the length of each path.
    /// \return The length of each path.
    [[nodiscard]] int PathLength() const noexcept {
        return Values.extent(1);
    }

    /// \brief Returns the duration of each path in seconds.
    /// \return The duration of each path in seconds.
    [[nodiscard]] double DurationSeconds() const noexcept {
        return PathLength() * IntervalSeconds;
    }

    /// \brief Returns the i-th path in the collection.
    /// \param i The index of the path.
    /// \return The i-th path in the collection.
    TimeSeriesView<T> operator[](int i) const {
        return {IntervalSeconds, ToSpan(submdspan(Values, i, full_extent))};
    }
};
